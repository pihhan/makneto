
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

#define RANDOM_ID_LENGTH 8

extern GMainLoop    *loop;

typedef std::pair<JingleSession *, JingleManager *> SessionManagerPair;

#ifdef GLOOX
using namespace gloox;
#endif

static gboolean periodic_timer(gpointer user_data)
{
    JingleManager *manager = (JingleManager *) user_data;
    JingleManager::SessionMap all = manager->allSessions();
    bool active = false;
    for (JingleManager::SessionMap::iterator it= all.begin(); it!= all.end(); it++) {
        JingleSession *session = (it->second);
        if (session && (session->state() == JSTATE_ACTIVE 
                     || session->state() == JSTATE_PENDING) ) {
            manager->periodicSessionCheck(session);
            active = true;
        }
    }
    return (TRUE);
}


JingleManager::JingleManager()
	: m_timerid(0),m_stunPort(0)
{
        m_seed = (unsigned int) time(NULL);
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

JingleSession * JingleManager::initiateEmptySession(
        const PJid &to, 
        const PJid &initiator) 
{
	JingleSession *session = new JingleSession();
        if (!session) 
            return NULL;
        session->setSid(randomId());
        session->setAction(ACTION_INITIATE);
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
    const PJid &to, 
    const PJid &initiator)
{
    JingleSession *session = initiateEmptySession(to, initiator);
    session->addLocalContent(audioContent());
    JingleStanza *js = session->createStanzaInitiate();

    addSession(session);
   
    send(js); 
    delete js;

    return session;
}


/** @brief Accept audio session. 
    create multimedia pipeline also. 
*/
JingleSession * JingleManager::acceptAudioSession(JingleSession *session)
{
    JingleContent newcontent = audioContent();

    session->addLocalContent(newcontent);
    setState(session, JSTATE_ACTIVE);
    session->setResponder(self());
    JingleStanza *js = session->createStanzaAccept();


    FstJingle *fsj = new FstJingle();
    fsj->createAudioSession(session);
    session->setData(fsj);
    addSession(session);

    send(js);
    // start timeout timer
    startSessionTimeout(session);
    startPeriodicTimer();
    return session;
}

/** @brief Internal handle of confirmation or our outgoing call. */
bool JingleManager::acceptedAudioSession(JingleSession *session)
{
    setState(session, JSTATE_ACTIVE);
    
    FstJingle *fsj = new FstJingle();
    fsj->createAudioSession(session);

    session->setData(fsj);

    startSessionTimeout(session);
    startPeriodicTimer();
    return true;
}


/** @brief Send termination of this session over network, and shutdown this
    session. 
*/
void JingleManager::terminateSession(JingleSession *session, SessionReason reason)
{
    if (session->state() == JSTATE_TERMINATED) {
        LOGGER(logit) << "Request to terminate already terminated session" << std::endl;
        return;
    }

    JingleStanza *stanza = session->createStanzaTerminate(reason);
    send(stanza);
    setState(session, JSTATE_TERMINATED);
    if (session->data()) {
        FstJingle *fst = static_cast<FstJingle *>(session->data());
        fst->terminate();
        LOGGER(logit) << "Audio session found and terminated." << std::endl;
    } else {
        LOGGER(logit) << "No data for session " << session->sid() << std::endl;
    }
}

/** @brief Return simple local candidates.
    Get them by enumerating local interfaces for addreses. */
CandidateList JingleManager::localUdpCandidates()
{
	CandidateList cl;
	char ** charlist = getAddressList(NULL, AFB_INET, SCOPE_GLOBAL);
	if (!charlist) {
		std::cerr << "Chyba pri ziskavani lokalnich adres." << std::endl;
		return cl;
	}
    int port = randomPort();

	for (char **it=charlist; it && *it; it++) {
		JingleUdpCandidate candidate;
		candidate.ip = std::string(*it);
		candidate.port = port;
		candidate.id = randomId();
		candidate.component = 1;
		candidate.generation = 0;
		candidate.natType = JingleCandidate::NAT_NONE;
		cl.push_back(candidate);

        // pridam taky RTCP kanal
        JingleUdpCandidate rtcp_cnd;
        rtcp_cnd.ip = std::string(*it);
        rtcp_cnd.port = candidate.port+1;
        rtcp_cnd.id = randomId();
        rtcp_cnd.component = 2;
        rtcp_cnd.generation = 0;
        rtcp_cnd.natType = JingleCandidate::NAT_NONE;
        cl.push_back(rtcp_cnd);
	}
	v6Destroylist(charlist);
	return cl;
}

JingleTransport JingleManager::localTransport()
{
	JingleTransport t;
	t.m_xmlns = XMLNS_JINGLE_RAWUDP;
	t.m_ufrag = randomId();
	t.m_pwd = randomId();
	t.candidates = localUdpCandidates();
	return t;
}

JingleTransport JingleManager::emptyUdpTransport()
{
    JingleTransport t;
    t.m_xmlns = XMLNS_JINGLE_RAWUDP;
    t.m_ufrag = randomId();
    t.m_pwd = randomId();
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
    static const char base[] = "01234567890abcdefghijklmnopqrstuvwxyz";
    std::string id;
    for (int len = 0; len<RANDOM_ID_LENGTH; len++) {
        int i = rand_r(&m_seed) % (sizeof(base)-1);
        id.append(1, base[i]);
    }
    return id;
}

JingleRtpContentDescription	JingleManager::audioDescription()
{
	JingleRtpContentDescription d;
	d.m_xmlns = "urn:xmpp:jingle:apps:rtp:1";
    d.addPayload(JingleRtpPayload(0, "PCMU", 8000));
    d.addPayload(JingleRtpPayload(8, "PCMA", 8000));
    d.addPayload(JingleRtpPayload(2, "G721", 8000));
    d.addPayload(JingleRtpPayload(3, "GSM", 8000));
#ifdef USE_SPEEX
	d.addPayload(JingleRtpPayload(97, "speex", 8000));
	d.addPayload(JingleRtpPayload(96, "speex", 16000));
#endif
	return d;
}

/** @brief Create Video description.
    @see http://www.iana.org/assignments/rtp-parameters
    For now, not configurable.
*/
JingleRtpContentDescription     JingleManager::videoDescription()
{
    JingleRtpContentDescription d;
    d.addPayload(JingleRtpPayload(96, "H263-1998", 90000));
    d.addPayload(JingleRtpPayload(97, "H263-2000", 90000));
    d.addPayload(JingleRtpPayload(98, "theora", 90000));

    return d;
}

JingleContent   JingleManager::audioContent()
{
    JingleContent c(localTransport(), audioDescription() );
    c.setName("audiotest");
    return c;
}

JingleContent   JingleManager::videoContent()
{
    JingleContent c(localTransport(), videoDescription() );
    c.setName("videotest");
    return c;
}

/** @brief Periodic check for new information, state change, something like 
    that. */
void JingleManager::periodicSessionCheck(JingleSession *session)
{
    FstJingle *fst = static_cast<FstJingle *>(session->data());
    if (fst) {
        if (fst->haveNewLocalCandidates()) {
            ContentList cl = session->localContents();
            ContentList modified;

            for (ContentList::iterator it=cl.begin(); it!=cl.end(); it++) {
                if (fst->updateLocalTransport(*it)) {
                    commentSession(session, "Local Transport updated: " + it->name());
                    modified.push_back(*it);
                }
            }

            if (modified.size() > 0) {
                JingleStanza *js = session->createStanza(ACTION_TRANSPORT_INFO);
                js->replaceContents(modified);
                send(js);

                session->replaceLocalContent(cl);
            }
            fst->resetNewLocalCandidates();
        } // new candidates
    } // fst
}


void JingleManager::startPeriodicTimer()
{
    if (m_timerid != 0)
        stopPeriodicTimer();

    m_timerid = g_timeout_add(PERIODIC_TIMEOUT, periodic_timer, this);
}

void JingleManager::stopPeriodicTimer()
{
    if (m_timerid != 0)
        g_source_remove(m_timerid);
    m_timerid = 0;
}

bool JingleManager::runningPeriodicTimer()
{
    return (m_timerid != 0);
}

void JingleManager::registerActionHandler(JingleActionHandler *handler)
{
    m_handler = handler;
}

JingleManager::SessionMap  JingleManager::allSessions()
{
    return m_sessions; 
}
        

/** @brief Called on timeout, tries next in candidate list.  
    That usually means that candidates we used could not connect. 
    This is needed for RAWUDP transport, as it cannot handle
    more than one remote candidates at one time. */
bool JingleManager::sessionTimeout(JingleSession *session)
{
    LOGGER(logit) << "session timeout sid: " << session->sid() << std::endl;

    if (session->state() == JSTATE_TERMINATED) {
        return FALSE;
    }
    if (session->state() == JSTATE_ACTIVE) {
        FstJingle *fst = static_cast<FstJingle*>(session->data());
        if (fst && fst->isPlaying())
            // pipeline is active, connection was successful
            return FALSE;
    }

    bool untried = false;
    ContentList cl = session->remoteContents();
    for (ContentList::iterator it=cl.begin(); it!= cl.end(); it++) {
        FstJingle *fst = static_cast<FstJingle *>(session->data());
        if (fst->tryNextCandidate(*it)) {
            untried = true;
        }
    } // for contents
    // place change in availability in clients back to session
    session->replaceRemoteContent(cl);


    if (!untried) {
        LOGGER(logit) << "No more candidates remains, " 
            << " session: " << session->sid() 
            << std::endl;
        terminateSession(session, REASON_MEDIA_ERROR);
        return FALSE;
    }
    return TRUE;
}

void JingleManager::startSessionTimeout(JingleSession *session)
{
    SessionManagerPair *p = new SessionManagerPair(session, this);
    g_timeout_add(CANDIDATE_TIMEOUT_MS, JingleManager::sessionTimeout_gcb, p);
}

/** @brief Callback for glib, maps static call back to object method. 
    @param user_data Pointer to std::pair<JingleSession *, JingleManager *>
*/
gboolean JingleManager::sessionTimeout_gcb(gpointer user_data)
{
    SessionManagerPair *p = static_cast<SessionManagerPair *>(user_data);
    g_assert(p->first);
    JingleManager *manager = p->second;
    JingleSession *session = p->first;
    bool r = manager->sessionTimeout(session);
    if (!r)
        delete p;
    return r;
}

/** @brief Will remove session from list, and free all its data. */
void JingleManager::destroySession(JingleSession *session)
{
    m_sessions.erase(session->sid());
    FstJingle *fst = static_cast<FstJingle *>(session->data());
    delete fst;
    session->setData(NULL);
    delete session;
}


/** @brief Modify session from remote party information in stanza. */
void JingleManager::modifySession(JingleSession *session, JingleStanza *stanza)
{
    FstJingle *fst = static_cast<FstJingle *>(session->data());
    LOGGER(logit) << " modify session sid:" << session->sid() << std::endl;
    switch (stanza->action()) {
        case ACTION_TRANSPORT_INFO: // modify transport parameters, or codecs
            if (fst) {
                ContentList contents = stanza->contents();
                for (ContentList::iterator it = contents.begin(); 
                        it != contents.end(); it++) 
                {
                    fst->replaceRemoteContent(*it);
                }
            }
            break;
        default:
            LOGGER(logit) << "unhandled modify session action "
                    << stanza->action() << std::endl;
            break;
    }
}

void JingleManager::setError(const std::string &errmsg)
{
    LOGGER(logit) << errmsg << std::endl;
}

void JingleManager::setStun(const std::string &ip, int port)
{
    m_stunIp = ip;
    m_stunPort = port;
}

std::string JingleManager::stunIp() const
{
    return m_stunIp;
}

int JingleManager::stunPort() const
{
    return  m_stunPort;
}

void JingleManager::setState(JingleSession *session, SessionState state)
{
    session->setState(state);
}



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


