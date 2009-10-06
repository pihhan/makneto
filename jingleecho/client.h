
#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include <gloox/gloox.h>
#include <gloox/presencehandler.h>
#include <gloox/messagehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/client.h>
#include <gloox/stanza.h>


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

    std::string authError();

    void sendChatMessage(const JID &to, const std::string &message);

    Client  *client() { return m_client; }

    private:
    Client  *m_client;
    JID     m_jid;
    
};

} // namespace gloox

#endif

