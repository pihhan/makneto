
#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include <gloox/gloox.h>
#include <gloox/presencehandler.h>
#include <gloox/messagehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/client.h>
#include <gloox/stanza.h>

#include "requests.h"
#include "versionhandler.h"

namespace gloox {

class EchoClient : public PresenceHandler, ConnectionListener, MessageHandler
{
    public:

    EchoClient(const std::string &jid, const std::string &password);

    virtual void handlePresence(Stanza *stanza);

    virtual void handleMessage (Stanza *stanza, MessageSession *session=0);

    virtual void onConnect();
    virtual void onDisconnect(ConnectionError e);
    virtual bool onTLSConnect( const CertInfo & info );

    void connect();
    void initLog();
    Stanza * bounceMessage(Stanza *stanza);

    static std::string connErrorToString(ConnectionError e);  
    static std::string authErrorToString(AuthenticationError e);
    static std::string stanzaErrorToString(StanzaError e);
	
	/** @brief Create text description of error from received stanza. */
	static std::string describeError(const Stanza *stanza);

    std::string authError();

    void sendChatMessage(const JID &to, const std::string &message);
#ifdef DNS_RESOLVE
	std::string resolveToString(const std::string &domain, const std::string &service = "", const std::string &proto="_tcp");
#endif


    Client  *client() { return m_client; }

	VersionIqHandler *versionHandler();

    private:
    Client  *m_client;
    JID     m_jid;
    RequestList *m_requests;
    VersionIqHandler *m_verhandler;
    
};

} // namespace gloox

#endif

