#include <string>
#include <iris/xmpp_message.h>
#include <xmpp_client.h>

#include "irisjinglemanager.h"
#include "logger/logger.h"

using namespace XMPP;

/* Default stun address. Used here, before i have better way to configure 
    stun address from resolver or configuration. */
static const std::string default_stun_ip = "212.71.150.10";

IrisJingleManager::IrisJingleManager(Task *parent)
    : XMPP::Task(parent), JingleManager::JingleManager()
{
    setStun(default_stun_ip);

    connect(this, SIGNAL(sessionIncoming(JingleSession*)), 
            this, SLOT(sessionAccept(JingleSession*)) );
}

/** @brief Send stanza over XMPP.
    @param js JingleStanza to be sent.
    Generate unique ID for new iq stanza, convert JingleStanza to XML stanza
    library cand send, and send over network.
*/
void IrisJingleManager::send(JingleStanza *js)
{
    // TODO: create sender Task, that will notify us on reception.
    std::string id = client()->genUniqueId().toStdString();
    QDomElement e = createJingleStanza(js, id, doc());
    XMPP::Task::send(e);
}

/** @brief Send terminate request to specified JID, with reason and session id
    from parameters.
    @param to Target JID, must be full JID.
    @param reason Reason why we terminate.
    @param sid Id of session to terminate.
*/
void IrisJingleManager::replyTerminate(const PJid &to, SessionReason reason, const std::string &sid)
{
    JingleStanza *js = new JingleStanza();
    js->setSid(sid);
    js->setTo(to);
    js->setAction(ACTION_TERMINATE);
    js->setReason(reason);

    send(js);
}

/** @brief Create small iq stanza to acknowledge stanza e.
    @param e XML iq stanza we are acknowledging.
    It will reverse from and to attributes, with id attribute same as e,
    and send.
*/
void IrisJingleManager::replyAcknowledge(const QDomElement &e)
{
    QDomElement iq = doc()->createElement("iq");
    iq.setAttribute("type", "result");
    iq.setAttribute("id", e.attribute("id"));
    iq.setAttribute("to", e.attribute("from"));
    iq.setAttribute("from", e.attribute("to"));
    XMPP::Task::send(iq);
}

PJid  IrisJingleManager::self()
{
    return client()->jid();
}


QDomElement IrisJingleManager::createJingleStanza(
    JingleStanza *js, 
    const std::string &id, 
    QDomDocument *doc)
{
    QDomElement iq = doc->createElement("iq");
    iq.setAttribute("to", js->to().full());
    iq.setAttribute("type", "set");
    iq.setAttribute("id", QString::fromStdString(id));
    iq.appendChild(js->tag(*doc));
    return iq;
}


void IrisJingleManager::commentSession(
    JingleSession *session, 
    const std::string &comment)
{
#if 0
    Message m(session->remote());
    m.setBody(QString::fromStdString(comment));
    client()->sendMessage(m);
#else 
    LOGGER(logit) << "Sid: " << session->sid() 
        << "Comment: " << comment << std::endl;
#endif
}

/*! \brief Reply to passed stanza with error response.
    \param e XML stanza with iq packet.
    \param errtag Tag name for child of error tag.
    \param type Type attribute of error tag. If empty, "cancel" is default.
*/
void IrisJingleManager::replyError(
    const QDomElement &e, 
    const QString &errtag,
    const QString &type)
{
    QString ltype = type;
    QDomElement iq = doc()->createElement("iq");
    iq.setAttribute("from", e.attribute("to"));
    iq.setAttribute("to", e.attribute("from"));
    iq.setAttribute("id", e.attribute("id"));
    iq.setAttribute("type", "error");
    QDomElement error = doc()->createElement("error");
    if (type.isEmpty())
        ltype = "cancel";
    error.setAttribute("type", ltype);
    QDomElement spec = doc()->createElement(errtag);
    spec.setAttribute("xmlns", "urn:ietf:params:xml:ns:xmpp-stanzas");
    iq.appendChild(error);

    XMPP::Task::send(iq);
}


/*! \brief Accept incoming stanza, if it is type=set and XMLNS is XMLNS_JINGLE
*/
bool IrisJingleManager::take(const QDomElement &e)
{
    QString type = e.attribute("type");
    QString from = e.attribute("from");
    QDomElement jingle = e.firstChildElement("jingle");
    if (jingle.isNull())
        return false;
    QString xmlns = jingle.attribute("xmlns");
    if (xmlns == XMLNS_JINGLE && type == "set" && e.tagName() == "iq") {
        QString sid = jingle.attribute("sid");
        std::string ssid = sid.toStdString();
        //JingleSession *session = getSession(ssid);
        JingleStanza *js = new JingleStanza();
        js->parse(e);
        JingleSession *session = getSession(js->sid());
        switch (js->action()) {
            case ACTION_INITIATE:
                if (session) {
                    logit << ("Prichozi initiate pro existujici session!") << std::endl;
                    replyError(e, "bad-request");
                } else {
                    session = new JingleSession();
                    session->initiateFromRemote(js);
                    replyAcknowledge(e);
                    session->setCaller(false);
                    emit sessionIncoming(session);
                    // FIXME: emulate accepting from gui, react on signal
                    //acceptAudioSession(session);
                }
                break;
            case ACTION_ACCEPT:
                if (session) {
                    replyAcknowledge(e);
                    session->mergeFromRemote(js);
                    acceptedAudioSession(session);
                    emit sessionStateChanged(session);
                } else {
                    logit << ("Jingle accept pro neexistujici session.") << std::endl;
                    replyError(e, "service-unavailable");
                }
                
                break;
            case ACTION_TERMINATE:
                if (session) {
                    replyAcknowledge(e);
                    setState(session, JSTATE_TERMINATED);
                    terminateSession(session);
                    emit sessionTerminated(session);
                } else {
                    replyError(e, "service-unavailable");

                    LOGGER(logit) << "Ukonceni session pro neznamou session: " 
                        << ssid << std::endl;
                }
                break;
            case ACTION_INFO:
                if (session) {
                    replyAcknowledge(e);
                    reportInfo(session, js->info());
                } else {
                    replyError(e, "service-unavailable");
                }
                break;
            case ACTION_TRANSPORT_INFO:
                if (session) {
                    replyAcknowledge(e);
                    modifySession(session, js);
                    emit sessionStateChanged(session);
                } else {
                    replyError(e, "bad-request");
                }
                break;
            default:
                LOGGER(logit) << "unhandled action " << js->action() << std::endl;
        }

        return true;
    } else {
        return false;
    }
}

void IrisJingleManager::onDisconnect()
{
    // TODO: clean active sessions
    LOGGER(logit) << "JingleManager task disconnected." << std::endl;
}

void IrisJingleManager::onGo()
{
    // we only receive, no sending here
}

void IrisJingleManager::setState(JingleSession *session, SessionState state)
{
    JingleManager::setState(session, state);
    emit sessionStateChanged(session);
}

void IrisJingleManager::sessionAccept(JingleSession *session)
{
    acceptAudioSession(session);
}

void IrisJingleManager::reportInfo(JingleSession *session, SessionInfo info)
{
    emit sessionInfo(session, info);
}
