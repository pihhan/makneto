
#ifndef JINGLEMANAGER_H
#define JINGLEMANAGER_H

#include <string>
#include <map>
#include <gloox/iqhandler.h>

#include "jinglesession.h"

/** @brief Abstract class to handle incoming requests. 
	Reimplement this to really do something with incoming jingle messages. 
*/
class JingleActionHandler
{
	public:
		
		virtual JingleSession::SessionReason handleNewSession(JingleSession *session) = 0;
		virtual JingleSession::SessionReason handleSessionAccept(JingleSession *session, JingleSession *update) = 0;
		virtual JingleSession::SessionReason handleSessionChange(JingleSession *session, JingleSession *update) = 0;
		virtual JingleSession::SessionReason handleSessionTermination(JingleSession *sesion) = 0;
		virtual JingleSession::SessionReason handleSessionError(JingleSession *session, JingleSession *update) = 0;
		
};

class JingleManager : public gloox::IqHandler
{
	public:
		typedef std::map<std::string, JingleSession *> 	SessionMap;
		
	JingleManager(gloox::ClientBase *base);
	
	virtual bool handleIq(gloox::Stanza *stanza);
	virtual bool handleIqID(gloox::Stanza *stanza);
	
	JingleSession * initiateAudioSession(const gloox::JID &from, const gloox::JID to);
	JingleSession * getSession(const std::string &sid);
	
	void 				removeSession(const std::string &sid);
	void				addSession(JingleSession *session);
	static std::string	getSid(gloox::Stanza *stanza);
	
	void registerActionHandler(JingleActionHandler *handler)
	{ m_handler = handler; }
	
	protected:
	
	SessionMap m_sessions;
	gloox::ClientBase *m_base;
	JingleActionHandler	*m_handler;
};


#endif
