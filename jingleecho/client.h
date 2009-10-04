
#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include <gloox/gloox.h>
#include <gloox/presencehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/client.h>
#include <gloox/stanza.h>


namespace gloox {

class EchoClient : public PresenceHandler, ConnectionListener
{
    public:

    EchoClient(const std::string &jid, const std::string &password);

    virtual void handlePresence(Stanza *stanza);

    virtual void onConnect();
    virtual bool onTLSConnect( const CertInfo & info );

    void connect();
    void initLog();

    private:
    Client  *m_client;
    JID     m_jid;
    
};

} // namespace gloox

#endif

