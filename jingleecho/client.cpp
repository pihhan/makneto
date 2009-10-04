
#include <iostream>
#include <gloox/logsink.h>
#include <gloox/jid.h>

#include "client.h"
#include "logger.h"

using namespace gloox;


EchoClient::EchoClient(const std::string &jid, const std::string &password)
{
    JID user(jid);
    
    m_client = new Client(user, password);
    m_client->registerPresenceHandler(this);
    m_client->registerConnectionListener(this);

    initLog();
}

void EchoClient::initLog()
{
    Logger::init();
    m_client->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, logger);
    m_client->logInstance().registerLogHandler(LogLevelWarning, LogAreaAll, logger);
    m_client->logInstance().registerLogHandler(LogLevelError, LogAreaAll, logger);
}

void EchoClient::handlePresence(Stanza *stanza)
{
    std::cout << "Received presence from " << stanza->from() << std::endl;
}

void EchoClient::onConnect()
{
    std::cout << "connected" << std::endl;
    m_client->logInstance().log(LogLevelDebug,LogAreaUser, "Connected.");
}

// TODO: alespon nejake overovani
bool EchoClient::onTLSConnect( const CertInfo &info )
{
    if (m_client->server() != info.server) {
        m_client->logInstance().log(LogLevelError, LogAreaUser, "Server certificate name and requested jid do not match.");
    }
    return info.chain;
}

void EchoClient::connect()
{
    m_client->connect();
}

