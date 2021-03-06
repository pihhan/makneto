
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
        if (session && (session->state() == JSTATE_PRECONFIGURE)) {
            if (manager->periodicPreconfigureCheck(session))
                active = true;
        }
    }
    return (TRUE);
}

JingleManager::JingleManager()
	: m_handler(0), m_seed(0), m_timerid(0),m_stunPort(0), m_mediaManager(0)
{
        m_seed = (unsigned int) time(NULL);
}

JingleManager::~JingleManager() 
{
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

/** @brief Create farsight session from jingle data, prepare
    input elements inside pipeline. */
bool JingleManager::prepareFstSession(
    JingleSession *session, 
    const MediaConfig &config)
{
    FstJingle *fst = static_cast<FstJingle *>(mediaManager());
    if (!fst) {
        fst = new FstJingle();
        setMediaManager(fst);
    }
    if (!fst || fst->lastError() != NoError) {
        reportError(session, fst->lastError(), fst->lastErrorMessage());
        return false;
    } else {
        fst->setMediaConfig(config);
        if (!m_stunIp.empty())
            fst->setStun(m_stunIp, m_stunPort);
        session->setData(fst);
        if (!fst->prepareSession(session)) {
            reportError(session, fst->lastError(), fst->lastErrorMessage());
            return false;
        }
        return true;
    }
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
    const PJid &initiator,
    const MediaConfig &config)
{
    JingleSession *session = initiateEmptySession(to, initiator);
    if (!session)
        return NULL;
    session->addLocalContent(audioContent());

    if (!prepareFstSession(session, config)) {
        return NULL;
    }

    setState(session, JSTATE_PRECONFIGURE);
    addSession(session);
  
    startSessionTimeout(session);
    startPeriodicTimer();
    return session;
}

JingleSession * JingleManager::initiateVideoSession(
    const PJid &to,
    const PJid &initiator,
    const MediaConfig &config)
{
    JingleSession *session = initiateEmptySession(to, initiator);
    session->addLocalContent(videoContent());

    if (!prepareFstSession(session, config)) {
        return NULL;
    }

    setState(session, JSTATE_PRECONFIGURE);
    addSession(session);
    startSessionTimeout(session);
    startPeriodicTimer();
    return session;
}

JingleSession * JingleManager::initiateAudioVideoSession(
    const PJid &to,
    const PJid &initiator,
    const MediaConfig &config)
{
    JingleSession *session = initiateEmptySession(to, initiator);
    session->addLocalContent(audioContent());
    session->addLocalContent(videoContent());

    if (!prepareFstSession(session, config)) {
        return NULL;
    }

    setState(session, JSTATE_PRECONFIGURE);
    addSession(session);
    startSessionTimeout(session);
    startPeriodicTimer();
    return session;
}

/** @brief Accept audio session. 
    create multimedia pipeline also. 
*/
JingleSession * JingleManager::acceptAudioSession(
    JingleSession *session, 
    const MediaConfig &config)
{
    ContentList rcl = session->remoteContents();
    for (ContentList::iterator it = rcl.begin(); it!=rcl.end(); it++) {
        JingleContent newcontent = audioContent();
        switch (it->description().type()) {
            case MEDIA_AUDIO:
                newcontent = audioContent();
                break;
            case MEDIA_VIDEO:
                newcontent = videoContent();
                break;
            case MEDIA_NONE:
            case MEDIA_AUDIOVIDEO:
                reportError(session, NegotiationError,
                    std::string("unknown type of content ") + it->name());
                break;
        }
        newcontent.setName(it->name()); // set the same name as remote.
        session->addLocalContent(newcontent);
    }

//    setState(session, JSTATE_ACTIVE);
    session->setResponder(self());
    session->setCaller(false);

    bool prepared = false;
    bool updated = false;
    FstJingle *fsj = static_cast<FstJingle *>(session->data());
    if (!fsj) {
        prepared = prepareFstSession(session, config);
        if (!prepared) {
            LOGGER(logit) << "prepareFstSession failed for session: " 
                << session->sid() << std::endl;
        }
        fsj = static_cast<FstJingle *>(session->data());
        updated = fsj->updateRemote(session);
        if (!updated) {
            LOGGER(logit) << "updateRemote failed for session: " 
                << session->sid() << std::endl;
        }
        //fsj->createAudioSession(session);
        //fsj->goPlaying();
    } else {
        LOGGER(logit) << "Accepting session with existing FstJingle pipeline!" 
            << std::endl;
    }
    setState(session, JSTATE_PRECONFIGURE);
    LOGGER(logit) << "fstjingle state: " << fsj->stateDescribe() << std::endl;
    addSession(session);

    // start timeout timer
    startSessionTimeout(session);
    startPeriodicTimer();
    return session;
}

/** @brief Internal handle of confirmation or our outgoing call. */
bool JingleManager::acceptedAudioSession(JingleSession *session)
{
    bool good = true;
    
    FstJingle *fsj = static_cast<FstJingle*>(mediaManager());
    if (!fsj) {
        fsj = new FstJingle();
        setMediaManager(fsj);
        good = fsj->createAudioSession(session);
        session->setData(fsj);
    } else {
        good = fsj->updateRemote(session);
        if (!good) {
           LOGGER(logit) << "updateRemote failed for session " 
                << session->sid() << std::endl; 
        }
    }
    setState(session, JSTATE_ACTIVE);
    good = good && fsj->goPlaying();

    LOGGER(logit) << "fstjingle state: " << fsj->stateDescribe() << std::endl;

    // startSessionTimeout(session);
    startPeriodicTimer();
    return good;
}


/** @brief Send termination of this session over network, and shutdown this
    session. 
*/
void JingleManager::terminateSession(JingleSession *session, SessionReason reason)
{
#if 0
        if (session->state() == JSTATE_TERMINATED) {
        LOGGER(logit) << "Request to terminate already terminated session" << std::endl;
        return;
    }
#endif


    if (session->state() == JSTATE_PENDING || session->state() == JSTATE_ACTIVE) {
        // only terminate on remote side if we sent already something
        JingleStanza *stanza = session->createStanzaTerminate(reason);
        send(stanza);
    }
    if (session->state() != JSTATE_TERMINATED)
        setState(session, JSTATE_TERMINATED);

    if (session->data()) {
        FstJingle *fst = static_cast<FstJingle *>(session->data());
        fst->terminate();
        // delete fst;
        LOGGER(logit) << "Session found and terminated." << std::endl;
    } else {
        LOGGER(logit) << "No data for session " << session->sid() << std::endl;
    }
    LOGGER(logit) << "Session " << session->sid() 
        << " terminated." << std::endl;
}

/** @brief Terminate all sessions on manager.
    Useful to terminate when quitting. */
void JingleManager::terminateAllSessions(SessionReason reason)
{
    for (SessionMap::iterator it = m_sessions.begin();
        it!=m_sessions.end(); it++) {
        terminateSession(it->second, reason);
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
	t.m_xmlns = XMLNS_JINGLE_ICE;
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
    d.m_xmlns = XMLNS_JINGLE_RTP;
//    d.addPayload(JingleRtpPayload(8, "PCMA", 8000));
#ifdef AUTODETECT_PAYLOAD
    d.addPayload(JingleRtpPayload(0, "PCMU", 8000));
    d.addPayload(JingleRtpPayload(8, "PCMA", 8000));
    d.addPayload(JingleRtpPayload(2, "G721", 8000));
    d.addPayload(JingleRtpPayload(3, "GSM", 8000));
#ifdef USE_SPEEX
    d.addPayload(JingleRtpPayload(97, "speex", 8000));
    d.addPayload(JingleRtpPayload(96, "speex", 16000));
#endif
#endif
    d.setType(MEDIA_AUDIO);
    return d;
}

/** @brief Create Video description.
    @see http://www.iana.org/assignments/rtp-parameters
    For now, not configurable.
*/
JingleRtpContentDescription     JingleManager::videoDescription()
{
    JingleRtpContentDescription d;
    d.addPayload(JingleRtpPayload(100, "H263-1998", 90000));
#ifdef AUTODETECT_PAYLOAD
    d.addPayload(JingleRtpPayload(97, "H264", 90000));
    d.addPayload(JingleRtpPayload(100, "H263-1998", 90000));
    d.addPayload(JingleRtpPayload(101, "H263-2000", 90000));
    d.addPayload(JingleRtpPayload(102, "theora", 90000));
#endif
    d.setType(MEDIA_VIDEO);
    return d;
}

JingleContent   JingleManager::audioContent()
{
    JingleContent c(localTransport(), audioDescription() );
    c.setName("audiotest");
    c.setMedia(MEDIA_AUDIO);
    return c;
}

JingleContent   JingleManager::videoContent()
{
    JingleContent c(localTransport(), videoDescription() );
    c.setName("videotest");
    c.setMedia(MEDIA_VIDEO);
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
                if (fst->updateLocalTransport(*it, session->remote().fullStd())) {
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

/** @brief Wait until preconfigure stage finishes.
    Terminate on error, send initiate on success. */
bool JingleManager::periodicPreconfigureCheck(JingleSession *session)
{
    FstJingle *fsj = static_cast<FstJingle *>(session->data());
    if (fsj) {
        LOGGER(logit) << "Preconfigure check" << std::endl;
        if (fsj->lastError() != NoError) {
            reportError(session, fsj->lastError(), fsj->lastErrorMessage()); 
            if (!session->localOriginated()) {
                terminateSession(session, REASON_MEDIA_ERROR);
            }
            session->setFailed(true);
            LOGGER(logit) << "Preconfigure failed for session " 
                << session->sid() << std::endl;
        } else if (fsj->isPreconfigured()) {
            FrameSizeList sizes = fsj->supportedVideoInputSizes();
            LOGGER(logit) << "Supported video sizes: " 
                << FrameSize::toString(sizes) << std::endl;
            if (session->needLocalPayload()) {
                bool updated = fsj->updateLocalDescription(session);
                LOGGER(logit) << "Updated local description " << updated 
                    <<std::endl;
            }
            JingleStanza *js = NULL;
            if (session->localOriginated()) {
                js = session->createStanzaInitiate();
                setState(session, JSTATE_PENDING);
            } else {
                js = session->createStanzaAccept();
                // TODO: set active only after acknowledge from remote
                fsj->goPlaying();
                setState(session, JSTATE_ACTIVE);
            }
            send(js); 
            delete js;
            LOGGER(logit) << "Preconfigure complete for session " 
                << session->sid() << std::endl;
        } else {
            LOGGER(logit) << "Not yet playing " 
                << " ready " << fsj->isReady() 
                << " paused " << fsj->isPaused() 
                << " playing " << fsj->isPlaying() 
                << " preconfigured " << fsj->isPreconfigured()
                << std::endl;
        }
        return true;
    } else {
        return false;
    }
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
    FstJingle *fst = static_cast<FstJingle *>(session->data());
    LOGGER(logit) << "Session " << session->sid() << " timed out, state: "
        << fst->stateDescribe() << std::endl;

    for (ContentList::iterator it=cl.begin(); it!= cl.end(); it++) {
        if (fst->tryNextCandidate(*it, session->remote().fullStd()) ) {
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
        reportFatalError(session, NetworkTimeoutError, "Session timed out.");
        return FALSE;
    }
    return TRUE;
}

void JingleManager::startSessionTimeout(JingleSession *session)
{
    SessionManagerPair *p = new SessionManagerPair(session, this);
    guint id = 
        g_timeout_add(CANDIDATE_TIMEOUT_MS, JingleManager::sessionTimeout_gcb, p);
    m_timers.insert( std::make_pair(session->sid(), id));
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
    UIntMap::iterator timer = m_timers.find(session->sid());
    if (timer != m_timers.end()) {
        g_source_remove(timer->second);
    }
    if (fst)
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
                    fst->replaceRemoteContent(*it, session->remote().fullStd());
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
    LOGGER(logit) << "Setting session " << session->sid() 
        << "state to " << state << std::endl;
    session->setState(state);
}

void JingleManager::reportFailed(JingleSession *session)
{
    LOGGER(logit) << "Session " << session->sid() << " failed!" << std::endl;
}

void JingleManager::reportError(JingleSession *session, JingleErrors error, const std::string &msg)
{
    LOGGER(logit) << "ERROR: Session " << session->sid() << ": " 
            << msg << " (" << error << ")" << std::endl;
    if (m_handler)
        m_handler->handleSessionError(session, error, msg);
}

void JingleManager::reportFatalError(JingleSession *session, JingleErrors error, const std::string &msg)
{
    LOGGER(logit) << "FATAL ERROR: Session " << session->sid() << ": " 
            << msg << " (" << error << ")" << std::endl;
    if (m_handler)
        m_handler->handleSessionError(session, error, msg);
}

void JingleManager::reportInfo(JingleSession *session, SessionInfo info)
{
    LOGGER(logit) << "SESSION INFO: " << session->sid() << " : " 
        << JingleSession::stringFromInfo(info) << std::endl;
}

/** @brief Set media manager to use. */
void JingleManager::setMediaManager(void *manager)
{
    m_mediaManager = manager;
}

void * JingleManager::mediaManager() const
{
    return m_mediaManager;
}


