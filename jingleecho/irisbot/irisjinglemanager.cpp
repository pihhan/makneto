#include <string>
#include <iris/xmpp_message.h>
#include <xmpp_client.h>

#include "irisjinglemanager.h"
#include "logger/logger.h"

using namespace XMPP;

IrisJingleManager::IrisJingleManager(Task *parent)
    : XMPP::Task(parent)
{
}

void IrisJingleManager::send(JingleStanza *js)
{
    // TODO: create sender Task, that will notify us on reception.
    std::string id = client()->genUniqueId().toStdString();
    QDomElement e = createJingleStanza(js, id, doc());
    XMPP::Task::send(e);
}

void IrisJingleManager::replyTerminate(const PJid &to, SessionReason reason, const std::string &sid)
{
    JingleStanza *js = new JingleStanza();
    js->setSid(sid);
    js->setTo(to);
    js->setAction(ACTION_TERMINATE);
    js->setReason(reason);

    send(js);
}

void IrisJingleManager::replyAcknowledge(const QDomElement &e)
{
    QDomElement iq = doc()->createElement("id");
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
    Message m(session->remote());
    m.setBody(QString::fromStdString(comment));
    client()->sendMessage(m);
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
                    acceptAudioSession(session);
                }
                break;
            case ACTION_ACCEPT:
                if (session) {
                    replyAcknowledge(e);
                } else {
                    logit << ("Jingle accept pro neexistujici session.") << std::endl;
                    replyError(e, "service-unavailable");
                }
                
                break;
            case ACTION_TERMINATE:
                if (session) {
                    terminateSession(session);
                    replyAcknowledge(e);
                } else {
                    replyError(e, "service-unavailable");

                    LOGGER(logit) << "Ukonceni session pro neznamou session: " 
                        << ssid << std::endl;
                }
                break;
            case ACTION_INFO:
                if (session) {
                    replyAcknowledge(e);
                } else {
                    replyError(e, "bad-request");
                }
                break;
        }

        return true;
    } else {
        return false;
    }
}


