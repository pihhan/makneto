
#include <cstdlib>
#include <iostream>
#include <ctime>

#include <gloox/stanza.h>
#include <gloox/disco.h>

#include "jinglemanager.h"
#include "fsjingle.h"

#include "ipv6/v6interface.h"

using namespace gloox;

JingleManager::JingleManager(ClientBase *base)
	: m_base(base)
{
	base->registerIqHandler(this, XMLNS_JINGLE);
        base->disco()->addFeature(XMLNS_JINGLE);
        base->disco()->addFeature(XMLNS_JINGLE_RAWUDP);
        base->disco()->addFeature(XMLNS_JINGLE_RTP);
        m_seed = (unsigned int) time(NULL);
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
	
	switch (stanza->subtype()) {
		case StanzaIqGet:
			break;
		case StanzaIqSet:
			local_session = getSession(sid);
			if (!session)
				session = new JingleSession(m_base);
			jingle = stanza->findChild("jingle", "xmlns", XMLNS_JINGLE);
			if (jingle) {
				session->parse(stanza, true);
				switch (session->action()) {
					case JingleSession::ACTION_INITIATE:
                                                session->setState(JingleSession::JSTATE_PENDING);
						addSession(session);
						if (m_handler) {
							replyAcknowledge(stanza);
							result = m_handler->handleNewSession(session);
						} else return false;
						break;
					case JingleSession::ACTION_ACCEPT:
						if (!local_session) {
							std::cerr << "Jingle accept pro neexistujici session" << std::endl;
							return false;
						}
						local_session->mergeSession(session, true);
                                                acceptedAudioSession(local_session);
						if (m_handler) {
							replyAcknowledge(stanza);
							result = m_handler->handleSessionAccept(local_session, session);
						} else return false;
						break;
					case JingleSession::ACTION_TERMINATE:
						if (m_handler && local_session) {
                                                    local_session->setState(JingleSession::JSTATE_TERMINATED);
						    replyAcknowledge(stanza);
						    result = m_handler->handleSessionTermination(local_session);
						} else return false;
						
					default:
						break;
						
				}
			} else { // if jingle
                            Tag * query = stanza->findChild("query");
                            if (query) 
                                std::cerr << "Query found inside jingle request" << std::endl;
				return false;
			}
			break;
		case StanzaIqResult:
			sid = getSid(stanza);
			local_session = getSession(sid);
			if (local_session)
				local_session->setAcknowledged(true);
			break;
		case StanzaIqError:
			if (!local_session) {
				std::cerr << "Chyba pro session, kterou neobsluhujeme! id:"
                                << sid << std::endl;
				return false;
			}
			if (m_handler) {
				result = m_handler->handleSessionError(local_session, stanza);
			}
                        return true;
		default:
			return false;
	}
	return true;
}


void JingleManager::addSession(JingleSession *session)
{
	m_sessions.insert(make_pair(session->sid(), session));
}

JingleSession * JingleManager::getSession(const std::string &sid)
{
	SessionMap::iterator it = m_sessions.find(sid);
	if (it!= m_sessions.end())
		return it->second;
	else return NULL;
}

void JingleManager::removeSession(const std::string &sid)
{
	m_sessions.erase(sid);
}

/** @brief Create iq stanza with jingle child instead of query. */
Stanza * JingleManager::createJingleStanza(const gloox::JID &to, const std::string &id, enum StanzaSubType type, Tag *jingle)
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

JingleSession * JingleManager::initiateEmptySession(
        const gloox::JID &to, 
        const gloox::JID &initiator) 
{
	JingleSession *session = new JingleSession(m_base);
        if (!session) 
            return NULL;
        session->setSid(randomId());
        session->setAction(JingleSession::ACTION_INITIATE);
//	session->initiateAudioSession(from, to);
        session->setSelf(self());
        session->setCaller(true);
        session->setRemote(to);
        session->setTo(to);
        session->setFrom(self());
        if (initiator)
            session->setInitiator(initiator);
        return session;
}

JingleSession * JingleManager::initiateAudioSession(
    const gloox::JID &to, 
    const gloox::JID &initiator)
{
        JingleSession *session = initiateEmptySession(to, initiator);
        session->addLocalContent(audioContent());
	Tag *jingle = session->tag(JingleSession::ACTION_INITIATE);
	addSession(session);
	
	std::string id = m_base->getID();
	Stanza *stanza = createJingleStanza(to, id, StanzaIqSet, jingle);
	m_base->send(stanza);
        return session;
}


JingleSession * JingleManager::acceptAudioSession(JingleSession *session)
{
	JingleSession *ls = JingleSession::createReply(session);
        if (!ls)
            return NULL;
        JingleContent newcontent = audioContent();

        ls->setSelf(self());
        ls->setRemote(session->from());
        ls->setAction(JingleSession::ACTION_ACCEPT);
        ls->addLocalContent(newcontent);
        ls->replaceRemoteContent(session->remoteContents());
        ls->setState(JingleSession::JSTATE_ACTIVE);
        session->addLocalContent(newcontent);
        session->setState(JingleSession::JSTATE_ACTIVE);
        session->setResponder(self());

	Tag *jingle = ls->tag();

        FstJingle *fsj = new FstJingle();
        fsj->createAudioSession(session);
        ls->setData(fsj);

//        addSession(ls);	
	std::string id = m_base->getID();
	Stanza *stanza = createJingleStanza(ls->to(), id, StanzaIqSet, jingle);
	m_base->send(stanza);
        return ls;
}

/** @brief Internal handle of confirmation or our outgoing call. */
bool JingleManager::acceptedAudioSession(JingleSession *session)
{
    session->setState(JingleSession::JSTATE_ACTIVE);
    
    FstJingle *fsj = new FstJingle();
    fsj->createAudioSession(session);

    session->setData(fsj);
    return true;
}

/** @brief Create result reply to specified stanza with matching id. */
void JingleManager::replyAcknowledge(const Stanza *stanza)
{
	if (stanza->subtype() == StanzaIqSet || stanza->subtype() == StanzaIqGet) {
		Stanza *reply = Stanza::createIqStanza(stanza->from(), stanza->id(), StanzaIqResult);
		m_base->send(reply);
	}
}

/** @brief Create result reply to specified stanza with matching id. */
void JingleManager::replyTerminate(const Stanza *stanza, JingleSession::SessionReason reason, const std::string &sid)
{
	if (stanza->subtype() == StanzaIqSet || stanza->subtype() == StanzaIqGet) {
		Tag *jingle = new Tag("jingle", "xmlns", XMLNS_JINGLE);
		jingle->addAttribute("action", "session-terminate");
		jingle->addAttribute("sid", sid);
		Tag *r = new Tag(jingle, "reason");
		Tag *reason_subtag = new Tag(r, 
                        JingleSession::stringFromReason(reason));
		
		std::string id = m_base->getID();
		Stanza *reply = Stanza::createIqStanza(stanza->from(), id, StanzaIqSet, XMLNS_JINGLE, jingle);
		m_base->send(reply);
	}
}



JingleTransport::CandidateList JingleManager::localUdpCandidates()
{
	JingleTransport::CandidateList cl;
	char ** charlist = getAddressList(NULL, AFB_INET, SCOPE_GLOBAL);
	if (!charlist) {
		std::cerr << "Chyba pri ziskavani lokalnich adres." << std::endl;
		return cl;
	}
	for (char **it=charlist; it && *it; it++) {
		JingleUdpCandidate candidate;
		candidate.ip = std::string(*it);
		candidate.port = randomPort();
		candidate.id = randomId();
		candidate.component = 0;
		candidate.generation = 0;
		candidate.natType = JingleCandidate::NAT_NONE;
		cl.push_back(candidate);
	}
	v6Destroylist(charlist);
	return cl;
}

JingleTransport JingleManager::localTransport()
{
	JingleTransport t;
	t.m_xmlns = "urn:xmpp:jingle:transports:ice-udp:1";
	t.m_ufrag = randomId();
	t.m_pwd = randomId();
	t.candidates = localUdpCandidates();
	return t;
}

/** @brief Get unprivileged random port. */
unsigned int JingleManager::randomPort()
{
	unsigned int port = (((unsigned int) rand_r(&m_seed)) % 32000) + 1024;
        return port;
}

std::string JingleManager::randomId()
{
	return m_base->getID();
}

JingleRtpContentDescription	JingleManager::audioDescription()
{
	JingleRtpContentDescription d;
	d.m_xmlns = "urn:xmpp:jingle:apps:rtp:1";
	d.addPayload(JingleRtpPayload(96, "speex", 16000));
	d.addPayload(JingleRtpPayload(97, "speex", 8000));
	return d;
}

JingleContent   JingleManager::audioContent()
{
    return JingleContent(localTransport(), audioDescription() );
}


