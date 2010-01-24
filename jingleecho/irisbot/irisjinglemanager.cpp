
#include <iris/xmpp_message.h>
#include "irisjinglemanager.h"

using namespace XMPP;

IrisJingleManager::IrisJingleManager(Task *parent)
    : XMPP::Task(parent);
{
}

virtual void IrisJingleManager::send(JingleStanza *js)
{
    // TODO: create sender Task, that will notify us on reception.
    std::string id = client()->getUniqueId().toStdString();
    send(createJingleStanza(js, id, doc()));
}

void IrisJingleManager::replyTerminate(const PJid &to, SessionReason reason, const std::string &sid="")
{
    JingleStanza *js = new JingleStanza();
    js->setSid(sid);
    js->setTo(to);
    js->setAction(ACTION_SESSION_TERMINATE);
    js->setReason(reason);

    send(js);
}

void IrisJingleManager::replyAcknowledge(const QDomElement &e)
{
    QDomElement iq = doc()->createElement("id");
    iq.addAttribute("type", "result");
    iq.addAttribute("id", e.attribute("id"));
    iq.addAttribute("to", e.attribute("from"));
    id.addAttribute("from", e.attribute("to"));
    send(id);
}

virtual PJid  IrisJingleManager::self()
{
    return client()->jid();
}

static QDomElement createJingleStanza(
    const PJid &to, 
    const std::string &id, 
    const std::string &type, 
    gloox::Tag *jingle, 
    QDomDocument *doc)
{

}


static QDomElement IrisJingleManager::createJingleStanza(
    JingleStanza *js, 
    const std::string &id, 
    QDomDocument *doc)
{
    QDomElement iq = doc->createElement("iq");
    iq.addAttribute("to", js->to().full());
    iq.addAttribute("type", "set");
    iq.addAttribute("id", QString::fromStdString(id));
    iq.appendChild(js->tag(*doc));
    return iq;
}


virtual void IrisJingleManager::commentSession(
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
    const QString &errtag
    const QString &type)
{
    QDomElement iq = doc()->createElement("iq");
    iq.addAttribute("from", e.attribute("to"));
    iq.addAttribute("to", e.attribute("from"));
    iq.addAttribute("id", e.attribute("id"));
    iq.addAttribute("type", "error");
    QDomElement error = doc()->createElement("error");
    if (type.isEmpty())
        type = "cancel";
    error.addAttribute("type", type);
    QDomElement spec = doc()->createElement(errtag);
    spec.addAttribute("xmlns", "urn:ietf:params:xml:ns:xmpp-stanzas");
    iq.appendChild(error);

    send(iq);
}


/*! \brief Accept incoming stanza, if it is type=set and XMLNS is XMLNS_JINGLE
*/
virtual bool IrisJingleManager::take(const QDomElement &e)
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
        JingleSession *session = getSession(ssid);
        JingleStanza *js = new JingleStanza();
        js->parse(e);
        JingleSession *session = getSession(js->sid());
        switch (js->action()) {
            case ACTION_INITIATE:
                if (session) {
                    setError("Prichozi initiate pro existujici session!");
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
                    setError("Jingle accept pro neexistujici session.");
                    replyError(e, "service-unavailable");
                }
                
                break;
            case ACTION_TERMINATE:
                if (session) {
                    sessionTerminate(session);
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


