
#ifndef JINGLEMANAGER_H
#define JINGLEMANAGER_H

#include <string>
#include <map>
#include <gloox/gloox.h>
#include <gloox/iqhandler.h>
#include <gloox/stanza.h>

#include "jinglesession.h"

/** @brief Abstract class to handle incoming requests. 
	Reimplement this to really do something with incoming jingle messages. 
*/
class JingleActionHandler
{
	public:
		
		virtual SessionReason handleNewSession(JingleSession *session) = 0;
		virtual SessionReason handleSessionAccept(JingleSession *session, JingleSession *update) = 0;
		virtual SessionReason handleSessionChange(JingleSession *session, JingleSession *update) = 0;
		virtual SessionReason handleSessionTermination(JingleSession *sesion) = 0;
                /** @brief Handle error of session. Expect JingleSession might be null, if stanza did not contain session id. */
		virtual SessionReason handleSessionError(JingleSession *session, const gloox::Stanza *stanza) = 0;
		
};


/** @brief Create manager for jingle sessions. 
    @author Petr Mensik <pihhan@cipis.net> */
class JingleManager : public gloox::IqHandler
{
    public:
        typedef std::map<std::string, JingleSession *> 	SessionMap;
		
	JingleManager(gloox::ClientBase *base);
	
	virtual bool handleIq(gloox::Stanza *stanza);
	virtual bool handleIqID(gloox::Stanza *stanza, int content);
	
        /** @brief Initiate new audio session. */
	JingleSession * initiateAudioSession(const gloox::JID &to);

        /** @brief Accept new audio session. */
        JingleSession * acceptAudioSession(JingleSession *session);

        /** @brief Terminate passed session. */
        void            terminateSession(JingleSession *session, SessionReason reason = REASON_DECLINE);


        void replyTerminate(const gloox::JID &to, SessionReason reason, const std::string &sid="");
	JingleSession * getSession(const std::string &sid);
	
	void 			removeSession(const std::string &sid);
	void			addSession(JingleSession *session);
	static std::string	getSid(gloox::Stanza *stanza);
        void                    replyAcknowledge(const gloox::Stanza *stanza);
	
	void registerActionHandler(JingleActionHandler *handler)
	{ m_handler = handler; }

        SessionMap              allSessions()
        { return m_sessions; }

        gloox::JID  self()  { return m_base->jid(); }
	

	unsigned int randomPort();
	std::string	randomId();
	
	/** @brief Get list of IPs this machine has. */
	JingleTransport::CandidateList	    localUdpCandidates();
	JingleTransport			    localTransport();
    JingleTransport             emptyUdpTransport();
	
	JingleRtpContentDescription		audioDescription();
    JingleContent                   audioContent();


    static gloox::Stanza * createJingleStanza(const gloox::JID &to, const std::string &id, enum gloox::StanzaSubType type, gloox::Tag *jingle);
    static gloox::Stanza * createJingleStanza(JingleStanza *js, const std::string &id);

    JingleSession * initiateEmptySession(const gloox::JID &to, 
                const gloox::JID &initiator = gloox::JID()  );
    JingleSession * initiateAudioSession(const gloox::JID &to, 
                const gloox::JID &initiator=gloox::JID()    );
    
    void            send(JingleStanza *js);

	protected:
        bool acceptedAudioSession(JingleSession *session);
	
	SessionMap m_sessions;
	gloox::ClientBase *m_base;
	JingleActionHandler	*m_handler;
        unsigned int                 m_seed;
};


#endif
