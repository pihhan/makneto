
#include <gloox/stanza.h>
#include "jinglemanager.h"

using namespace gloox;

JingleManager::JingleManager(ClientBase *base)
	: m_base(base)
{
	base->registerIqHandler(XMLNS_JINGLE, this);
}

std::string JingleManager::getSid(Stanza *stanza)
{
	Tag *jingle = stanza->findChild("jingle", "xmlns", XMLNS_JINGLE);
	if (!jingle)
		return std::string();
	return jingle->findAttribute("sid");
}

bool JingleManager::handleIq(Stanza *stanza)
{
	return handleIqID(stanza, 0);
}

bool JingleManager::handleIqID(Stanza *stanza, int context)
{
	JingleSession *session = NULL;
	Tag *jingle = NULL;
	std::string sid;
	JingleSession *local_session = NULL;
	JingleSession::SessionReason result = JingleSession::REASON_UNDEFINED;
	
	sid = getSid(stanza);
	local_session = getSession(sid);
	
	switch (stanza->subtype) {
		case StanzaIqGet:
			break;
		case StanzaIqSet:
			sid = getSid(stanza);
			local_session = getSession(sid);
			if (!session)
				session = new JingleSession(m_base);
			jingle = stanza->findChild("jingle", "xmlns", XMLNS_JINGLE);
			if (jingle) {
				session->parse(jingle);
				switch (session->action()) {
					case JingleSession::ACTION_INITIATE:
						addSession(session);
						if (m_handler) {
							replyAcknowledge(stanza);
							result = m_handler->handleNewSession(session);
						} else return false;
						break;
					case JingleSession::ACTION_ACCEPT:
						local_session = getSession(sid);
						if (!local_session) {
							std::cerr << "Jingle accept pro neexistujici session" << std::endl;
							return false;
						}
						local_session->mergeSession(session, true);
						local_session->setState(JingleSession::JSTATE_ACTIVE);
						if (m_handler) {
							replyAcknowledge(stanza);
							result = m_handler->handleSessionAccept(local_session, session);
						} else return false;
						break;
					case JingleSession::ACTION_TERMINATE:
						if (m_handler && local_session) {
							replyAcknowledge(stanza);
							result = m_handler->handleSessionTermination(local_session);
						} else return false;
						
					default:
						break;
						
				}
			} else { // if jingle
				return false;
			}
			break;
		case StanzaIqResult:
			sid = getSid(stanza);
			local_session = getSession(sid);
			if (local_session)
				local_session->setAcknowledge(true);
			break;
		case StanzaIqError:
			if (!local_session) {
				std::cerr << "Chyba pro session, kterou neobsluhujeme!" << std::endl;
				return false;
			}
			if (m_handler) {
				result = m_handler->handleSessionError(stanza, local_session);
			}
			break;
		default:
			return false;
	}
	return true;
}


void JingleManager::addSession(JingleSession *session)
{
	m_sessions.insert(make_pair(session->sid(), session));
}

JingleSession * getSession(const std::string &sid)
{
	SessionMap::iterator it = m_sessions.find(sid);
	if (it!= m_sessions.end())
		return (*it);
	else return NULL;
}

void JingleManager::removeSession(const std::string &sid)
{
	m_sessions.erase(sid);
}

JingleSession * JingleManager::initiateAudioSession(const gloox::JID &from, const gloox::JID &to)
{
	JingleSession *session = new JingleSession(m_base);
	session->initiateAudioSession(from, to);
	Tag *jingle = session->tag(JingleSession::ACTION_INITIATE);
	addSession(session);
	
	std::string id = m_base->getID();
	Stanza *stanza = Stanza::createIqStanza(to, id, StanzaIqSet, XMLNS_JINGLE, jingle);
	m_base->send(stanza);
}


void JingleManager::acceptAudioSession(JingleSession *session)
{
	JingleSession *ls = new JingleSession(m_base);
	ls->acceptAudioSession(session);
	Tag *jingle = ls->tag(JingleSession::ACTION_ACCEPT);
	
	std::string id = m_base->getID();
	Stanza *stanza = Stanza::createIqStanza(session->initiator(), id, StanzaIqSet, XMLNS_JINGLE, jingle);
	m_base->send(stanza);
	
	session->setState(JingleSession::JSTATE_ACTIVE);
}

/** @brief Create result reply to specified stanza with matching id. */
void JingleManager::replyAcknowledge(const Stanza *stanza)
{
	if (stanza->subtype() == StanzaIqSet || stanza->subtype() == StanzaIqGet) {
		if (reason == JingleSession::REASON_SUCCESS) {
			Stanza *reply = Stanza::createIqStanza(stanza->from(), stanza->id(), StanzaIqResult);
			m_base->send(reply);
		} else {
		}
	}
}

/** @brief Create result reply to specified stanza with matching id. */
void JingleManager::replyTerminate(const Stanza *stanza, JingleSession::SessionReason reason, const std::string &sid="")
{
	if (stanza->subtype() == StanzaIqSet || stanza->subtype() == StanzaIqGet) {
		Tag *jingle = new Tag("jingle", "xmlns", XMLNS_JINGLE);
		jingle->addAttribute("action", "session-terminate");
		jingle->addAttribute("sid", sid);
		Tag *reason = new Tag(jingle, "reason");
		Tag *reason_tag = new Tag(reason, JingleSession::stringFromReason(reason));
		
		std::string id = m_base->getId();
		Stanza *reply = Stanza::createIqStanza(stanza->from(), id, StanzaIqSet, XMLNS_JINGLE, jingle);
		m_base->send(reply);
	}
}
