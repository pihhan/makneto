
#ifndef JINGLEMANAGER_H
#define JINGLEMANAGER_H

#include <string>
#include <map>

#ifdef GLOOX
#include <gloox/gloox.h>
#include <gloox/iqhandler.h>
#include <gloox/stanza.h>
#endif
#include <glib.h>

#include "jinglesession.h"
#include "jingleerror.h"
#include "mediaconfig.h"

#define CANDIDATE_TIMEOUT_MS    30000
#define PERIODIC_TIMEOUT    150

/** @brief Abstract class to handle incoming requests. 
    @author Petr Mensik <pihhan@cipis.net>
    Reimplement this to really do something with incoming jingle messages. 
    
*/
class JingleActionHandler
{
    public:
    virtual ~JingleActionHandler() {}
		
    virtual SessionReason handleNewSession(JingleSession *session) = 0;
    virtual SessionReason handleSessionAccept(JingleSession *session, JingleSession *update) = 0;
    virtual SessionReason handleSessionChange(JingleSession *session, JingleSession *update) = 0;
    virtual SessionReason handleSessionTermination(JingleSession *sesion) = 0;
#ifdef GLOOX
    /** @brief Handle error of session. Expect JingleSession might be null, if stanza did not contain session id. */
    virtual SessionReason handleSessionError(JingleSession *session, const gloox::Stanza *stanza) = 0;
#endif
    virtual void handleSessionError(JingleSession *session, JingleErrors err, const std::string &msg, bool fatal=true);
    /** @brief Get notification that preconfiguring is complete.
        That means, pipeline was created without error. */
    virtual void handleSessionPreconfigured(JingleSession *session);
		
};


/** @brief Create manager for jingle sessions. 
    @author Petr Mensik <pihhan@cipis.net> 
    Some methods are virtual, as it needs XMPP library specific implementation.
*/
class JingleManager
{
    public:
        typedef std::map<std::string, JingleSession *> 	SessionMap;
        typedef std::map<std::string, unsigned int>     UIntMap;
		
	JingleManager();
        virtual ~JingleManager();
	
	
        /** @brief Initiate new audio session. */
	JingleSession * initiateAudioSession(const PJid &to);

        /** @brief Accept new audio session. */
        JingleSession * acceptAudioSession(JingleSession *session, 
            const MediaConfig &config = MediaConfig());

        /** @brief Terminate passed session. */
        void    terminateSession(JingleSession *session, SessionReason reason = REASON_DECLINE);
        void    terminateAllSessions(SessionReason reason = REASON_SUCCESS);
        void    modifySession(JingleSession *session, JingleStanza *stanza);


	JingleSession * getSession(const std::string &sid);
	
	void 		removeSession(const std::string &sid);
	void		addSession(JingleSession *session);
	
	void registerActionHandler(JingleActionHandler *handler);
        bool prepareFstSession(JingleSession *session, 
            const MediaConfig &config = MediaConfig());

        SessionMap      allSessions();

        virtual PJid  self()=0;
	
	unsigned int randomPort();
	std::string	randomId();
	
	/** @brief Get list of IPs this machine has. */
	CandidateList	    localUdpCandidates();
	JingleTransport	    localTransport();
        JingleTransport     emptyUdpTransport();
	
	JingleRtpContentDescription	audioDescription();
        JingleContent                   audioContent();
        JingleRtpContentDescription     videoDescription();
        JingleContent                   videoContent();



    JingleSession * initiateEmptySession(const PJid &to, 
                const PJid &initiator = PJid()  );
    JingleSession * initiateAudioSession(const PJid &to, 
                const PJid &initiator=PJid(),
                const MediaConfig &config=MediaConfig());
    JingleSession * initiateVideoSession(const PJid &to,
                const PJid &initiator=PJid(),
                const MediaConfig &config=MediaConfig());
    JingleSession * initiateAudioVideoSession(const PJid &to,
                const PJid &initiator = PJid(),
                const MediaConfig &config=MediaConfig());
    
    virtual void    send(JingleStanza *js)=0;

    void periodicSessionCheck(JingleSession *session);
    bool periodicPreconfigureCheck(JingleSession *session);
    void startPeriodicTimer();
    void stopPeriodicTimer();
    bool runningPeriodicTimer();

    virtual void commentSession(JingleSession *session, const std::string &comment)=0;
    /** @brief Set state for this session. 
        @param session Session on which to change state.
        @param state New state for session.
        It can notify subsystems about state change, if reimplemented. 
    */
    virtual void setState(JingleSession *session, SessionState state);
    virtual void reportInfo(JingleSession *session, SessionInfo info);
    virtual void reportFailed(JingleSession *session);
    virtual void reportError(JingleSession *session, JingleErrors error, const std::string &msg);
    virtual void reportFatalError(JingleSession *session, JingleErrors error, const std::string &msg);

    bool sessionTimeout(JingleSession *session);
    void startSessionTimeout(JingleSession *session);
    void destroySession(JingleSession *session);

    void setError(const std::string &errmsg);

    void        setStun(const std::string &ip, int port = 0);
    std::string stunIp() const;
    int         stunPort() const;

    void setMediaManager(void *manager);
    void *mediaManager() const;

    protected:
    static gboolean sessionTimeout_gcb(gpointer user_data);
    bool acceptedAudioSession(JingleSession *session);
	
    SessionMap  m_sessions;
    UIntMap     m_timers;
    JingleActionHandler *m_handler;
    unsigned int        m_seed;
    unsigned int        m_timerid;
    std::string         m_stunIp;
    int                 m_stunPort;
    void                *m_mediaManager;
};



#endif
