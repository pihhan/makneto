
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <utility>

#ifdef GLOOX
#include <gloox/stanza.h>
#include <gloox/disco.h>
#include <gloox/clientbase.h>
#endif

#include <glib.h>

#include "jinglemanager.h"
#include "fstjingle.h"

#include "ipv6/v6interface.h"
#include "logger/logger.h"

#include "glooxjinglemanager.h"

#ifdef GLOOX
using namespace gloox;
#endif

#ifdef GLOOX
/*
 *
 * GlooxJingleManager
 *
 */
GlooxJingleManager::GlooxJingleManager(ClientBase *base)
    : m_base(base)
{
	base->registerIqHandler(this, XMLNS_JINGLE);
        base->disco()->addFeature(XMLNS_JINGLE);
        base->disco()->addFeature(XMLNS_JINGLE_RAWUDP);
        base->disco()->addFeature(XMLNS_JINGLE_RTP);
}

bool GlooxJingleManager::handleIq(Stanza *stanza)
{
	return handleIqID(stanza, 0);
}

bool GlooxJingleManager::handleIqID(Stanza *stanza, int context)
{
	JingleSession *session = NULL;
	std::string sid;
	JingleSession *local_session = NULL;
	SessionReason result = REASON_UNDEFINED;
    JingleStanza *js = NULL;
	
	sid = getSid(stanza);
	local_session = getSession(sid);
    if (!sid.empty()) {
        js = new JingleStanza();
        js->parse(stanza);
    }
	
    switch (stanza->subtype()) {
            case StanzaIqGet:
                LOGGER(logit) << "Obdrzeno Get pro jingle query od " 
                    << stanza->from() << std::endl;
                return false;
                    break;
            case StanzaIqSet:
                    local_session = getSession(sid);
                    if (js && js->valid()) {
                            switch (js->action()) {
                                    case ACTION_INITIATE:
                                    session = new JingleSession();
                                    session->initiateFromRemote(js);
                                    setState(session, JSTATE_PENDING);
                                            addSession(session);
                                            if (m_handler) {
                                                    replyAcknowledge(stanza);
                                                    result = m_handler->handleNewSession(session);
                                            } else return false;
                                            break;
                                    case ACTION_ACCEPT:
                                            if (!local_session) {
                                                    LOGGER(logit) << "Jingle accept pro neexistujici session: " << sid << std::endl;
                                                    return false;
                                            }
                                            local_session->mergeFromRemote(js);
                    acceptedAudioSession(local_session);
                                            replyAcknowledge(stanza);
                                            if (m_handler) {
                                                    result = m_handler->handleSessionAccept(local_session, session);
                                            }
                                            break;
                                    case ACTION_TERMINATE:
                                            if (m_handler && local_session) {
                                                setState(local_session, JSTATE_TERMINATED);
                                                replyAcknowledge(stanza);
                                                if (m_handler)
                                                    result = m_handler->handleSessionTermination(local_session);
                                            } else return false;

                case ACTION_CONTENT_ADD:
                case ACTION_CONTENT_MODIFY:
                case ACTION_CONTENT_REJECT:
                                            
                                    default:
                                            break;
                                            
                            }
                    } else { // if jingle
            Tag * query = stanza->findChild("query");
            if (query) 
                LOGGER(logit) << "Query found inside jingle request" << std::endl;
                            return false;
                    }
                    break;
            case StanzaIqResult:
                    sid = getSid(stanza);
                    local_session = getSession(sid);
                    if (local_session)
                            local_session->setAcknowledged(true);
                    else {
                        LOGGER(logit) << "Potvrzeni pro neznamou jingle session: " 
                            << sid << std::endl;
                    }
                    break;
            case StanzaIqError:
                    if (!local_session) {
                            LOGGER(logit) << "Chyba pro session, kterou neobsluhujeme! id:"
                            << sid << std::endl;
                            return false;
                    }
                    if (m_handler) {
                            result = m_handler->handleSessionError(local_session, stanza);
                    } else {
            LOGGER(logit) << "Chyba od " << stanza->from() 
                << " neobslouzena, chybi handler" << std::endl;
        }
        return true;
            default:
                    return false;
    }
    return true;
}

/** @brief Send jingle stanza, so it is delivered back to me. */
void GlooxJingleManager::send(JingleStanza *js)
{
    std::string id = m_base->getID();
    Stanza *stanza = createJingleStanza(js, id);
    m_base->trackID(this, id, 5);
    m_base->send(stanza);
}

/** @brief Create iq stanza with jingle child instead of query. */
Stanza * GlooxJingleManager::createJingleStanza(const PJid &to, const std::string &id, enum StanzaSubType type, Tag *jingle)
{
        Tag *stanzatag = new Tag("iq");
        stanzatag->addAttribute("id", id);
        switch (type) {
            case StanzaIqGet:
                stanzatag->addAttribute("type", "get");
                break;
            case StanzaIqSet:
                stanzatag->addAttribute("type", "set");
                break;
            case StanzaIqResult:
                stanzatag->addAttribute("type", "result");
                break;
            case StanzaIqError:
                stanzatag->addAttribute("type", "error");
                break;
            default:
                return NULL;
        }
        if (to)
            stanzatag->addAttribute("to", to.full());
        stanzatag->addChild(jingle);
        Stanza *stanza = new Stanza(stanzatag);
        return stanza;
}


Stanza * GlooxJingleManager::createJingleStanza(JingleStanza *js, const std::string &id)
{
        Tag *t = js->tag();
        Stanza *stanza = createJingleStanza(js->to(), id, StanzaIqSet, t);
        return stanza;
}

/** @brief Create result reply to specified stanza with matching id. */
void GlooxJingleManager::replyAcknowledge(const Stanza *stanza)
{
    if (stanza->subtype() == StanzaIqSet || stanza->subtype() == StanzaIqGet) {
            Stanza *reply = Stanza::createIqStanza(stanza->from(), stanza->id(), StanzaIqResult);
            m_base->send(reply);
    }
}

/** @brief Create result reply to specified stanza with matching id. */
void GlooxJingleManager::replyTerminate(const PJid  &to, SessionReason reason, const std::string &sid)
{
    Tag *jingle = new Tag("jingle", "xmlns", XMLNS_JINGLE);
    jingle->addAttribute("action", "session-terminate");
    jingle->addAttribute("sid", sid);
    Tag *r = new Tag(jingle, "reason");
    new Tag(r, 
            JingleSession::stringFromReason(reason));
    
    std::string id = m_base->getID();
    Stanza *reply = Stanza::createIqStanza(to, id, StanzaIqSet, XMLNS_JINGLE, jingle);
    m_base->trackID(this, id, 3);
    m_base->send(reply);
}

PJid GlooxJingleManager::self()
{
    return m_base->jid();
}

void GlooxJingleManager::commentSession(JingleSession *session, const std::string &comment)
{
    PJid target;
    if (session->localOriginated())
        target = session->initiator();
    else 
        target = session->responder();
    if (target) {
        gloox::Stanza *s = Stanza::createMessageStanza(target, comment);
        m_base->send(s);
    }
}

std::string GlooxJingleManager::getSid(Stanza *stanza)
{
	Tag *jingle = stanza->findChild("jingle", "xmlns", XMLNS_JINGLE);
	if (!jingle)
		return std::string();
	return jingle->findAttribute("sid");
}

#endif


