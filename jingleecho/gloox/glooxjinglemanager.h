
#ifdef GLOOX

#ifndef GLOOXJINGLEMANAGER_H
#define GLOOXJINGLEMANAGER_H

#include "jinglemanager.h"

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
