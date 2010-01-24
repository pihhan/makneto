
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

#define CANDIDATE_TIMEOUT_MS    3000
#define PERIODIC_TIMEOUT    150

/** @brief Abstract class to handle incoming requests. 
    @author Petr Mensik <pihhan@cipis.net>
    Reimplement this to really do something with incoming jingle messages. 
    
*/
class JingleActionHandler
{
	public:
		
		virtual SessionReason handleNewSession(JingleSession *session) = 0;
		virtual SessionReason handleSessionAccept(JingleSession *session, JingleSession *update) = 0;
		virtual SessionReason handleSessionChange(JingleSession *session, JingleSession *update) = 0;
		virtual SessionReason handleSessionTermination(JingleSession *sesion) = 0;
#ifdef GLOOX
                /** @brief Handle error of session. Expect JingleSession might be null, if stanza did not contain session id. */
		virtual SessionReason handleSessionError(JingleSession *session, const gloox::Stanza *stanza) = 0;
#endif
		
};


/** @brief Create manager for jingle sessions. 
    @author Petr Mensik <pihhan@cipis.net> 
    Some methods are virtual, as it needs XMPP library specific implementation.
*/
class JingleManager
{
    public:
        typedef std::map<std::string, JingleSession *> 	SessionMap;
		
	JingleManager();
	
	
        /** @brief Initiate new audio session. */
	JingleSession * initiateAudioSession(const PJid &to);

        /** @brief Accept new audio session. */
        JingleSession * acceptAudioSession(JingleSession *session);

        /** @brief Terminate passed session. */
        void            terminateSession(JingleSession *session, SessionReason reason = REASON_DECLINE);


	JingleSession * getSession(const std::string &sid);
	
	void 			removeSession(const std::string &sid);
	void			addSession(JingleSession *session);
	
	void registerActionHandler(JingleActionHandler *handler);

        SessionMap              allSessions();

        virtual PJid  self()=0;
	

	unsigned int randomPort();
	std::string	randomId();
	
	/** @brief Get list of IPs this machine has. */
	CandidateList	    localUdpCandidates();
	JingleTransport	    localTransport();
        JingleTransport     emptyUdpTransport();
	
	JingleRtpContentDescription	audioDescription();
        JingleContent                   audioContent();



    JingleSession * initiateEmptySession(const PJid &to, 
                const PJid &initiator = PJid()  );
    JingleSession * initiateAudioSession(const PJid &to, 
                const PJid &initiator=PJid()    );
    
    virtual void    send(JingleStanza *js)=0;

    void periodicSessionCheck(JingleSession *session);
    void startPeriodicTimer();
    void stopPeriodicTimer();
    bool runningPeriodicTimer();

    virtual void commentSession(JingleSession *session, const std::string &comment)=0;
    bool sessionTimeout(JingleSession *session);
    void startSessionTimeout(JingleSession *session);
    void destroySession(JingleSession *session);

    void setError(const std::string &errmsg);

    protected:
    static gboolean sessionTimeout_gcb(gpointer user_data);
    bool acceptedAudioSession(JingleSession *session);
	
	SessionMap m_sessions;
	JingleActionHandler *m_handler;
        unsigned int        m_seed;
        unsigned int        m_timerid;
};


#ifdef GLOOX

/** @brief Wrap around for Gloox library. 
    This class is implementation of JingleManager using gloox. 
*/
class GlooxJingleManager :
    public gloox::IqHandler,
    public JingleManager
{
    public:
	GlooxJingleManager(gloox::ClientBase *base);

	virtual bool handleIq(gloox::Stanza *stanza);
	virtual bool handleIqID(gloox::Stanza *stanza, int content);
        virtual void    send(JingleStanza *js);
        void replyTerminate(const PJid &to, SessionReason reason, const std::string &sid="");
        void    replyAcknowledge(const gloox::Stanza *stanza);
        virtual PJid  self();
	static std::string	getSid(gloox::Stanza *stanza);
    
    static gloox::Stanza * createJingleStanza(const PJid &to, const std::string &id, enum gloox::StanzaSubType type, gloox::Tag *jingle);
    static gloox::Stanza * createJingleStanza(JingleStanza *js, const std::string &id);
    virtual void commentSession(JingleSession *session, const std::string &comment);

    protected:
	gloox::ClientBase *m_base;

};
#endif

#endif
