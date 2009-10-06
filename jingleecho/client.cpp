
#include <iostream>
#include <gloox/logsink.h>
#include <gloox/jid.h>
#include <gloox/disco.h>
#include <gloox/rostermanager.h>

#include "client.h"
#include "logger.h"
//#include "rosterator.h"

using namespace gloox;


EchoClient::EchoClient(const std::string &jid, const std::string &password)
{
    JID user(jid);
    
    m_client = new Client(user, password);
    m_client->registerPresenceHandler(this);
    m_client->registerConnectionListener(this);
    m_client->registerMessageHandler(this);

    m_client->disco()->setVersion("EchoClient", "alpha0");
    m_client->disco()->setIdentity( "client", "bot" );

    initLog();

    m_client->setPresence(PresenceChat, 5, "bot ready to ping-pong with you");
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

    m_client->rosterManager()->handlePresence(stanza);
}

void EchoClient::onConnect()
{
    std::cout << "connected" << std::endl;
    m_client->logInstance().log(LogLevelDebug,LogAreaUser, "Connected.");

    if (!m_client->authed()) {
        CL_DEBUG(m_client, "Not yet authenticated, starting...");
        m_client->login();
    }
}

// TODO: alespon nejake overovani
bool EchoClient::onTLSConnect( const CertInfo &info )
{
    if (m_client->server() != info.server) {
        m_client->logInstance().log(LogLevelError, LogAreaUser, "Server certificate name and requested jid do not match.");
    }
    // FIXME: pro zkouseni, at se dari
    //return info.chain;
    return true;
}

void EchoClient::connect()
{
    m_client->connect();
}

void EchoClient::onDisconnect(ConnectionError e)
{
    CL_DEBUG(m_client, "Disconnected because: " + connErrorToString(e));
}

Stanza * EchoClient::bounceMessage(Stanza *stanza)
{
    Stanza * reply = Stanza::createMessageStanza(
        stanza->from(),
        stanza->body(),
        stanza->subtype(),
        stanza->subject(),
        stanza->thread(),
        stanza->xmlLang());
    return reply;
}

void EchoClient::handleMessage (Stanza *stanza, MessageSession *session)
{
    std::string body = stanza->body();
    if (!body.empty()) {
        CL_DEBUG(m_client, "Message received: " + body);
        m_client->send(bounceMessage(stanza));
    }
}

std::string EchoClient::connErrorToString(ConnectionError e)
{
    switch (e) {
        case ConnNoError:
            return "success";
        case ConnStreamError:
            return "stream";
        case ConnStreamVersionError:
            return "stream-version";
        case ConnStreamClosed:
            return "stream-closed";
        case ConnProxyAuthRequired:
            return "proxy-auth-require";
        case ConnProxyAuthFailed:
            return "proxy-auth-failed";
        case ConnProxyNoSupportedAuth:
            return "proxy-auth-not-supported";
        case ConnIoError:
            return "io";
        case ConnParseError:
            return "parse";
        case ConnConnectionRefused:
            return "connection-refused";
        case ConnDnsError:
            return "dns";
        case ConnOutOfMemory:
            return "no-memory";
        case ConnNoSupportedAuth:
            return "unsupported-auth";
        case ConnTlsFailed:
            return "tls-failed";
        case ConnTlsNotAvailable:
            return "tls-unavailable";
        case ConnAuthenticationFailed:
            return "auth-failed";
        case ConnUserDisconnected:
            return "user-request";
        case ConnNotConnected:
            return "not-connected";
        default:
            return "unhandled";
    }
}

std::string EchoClient::authErrorToString(AuthenticationError e)
{
    switch (e) {
        case AuthErrorUndefined:
            return "undefined";
        case SaslAborted:
            return "sasl-aborted";
        case SaslIncorrectEncoding:
            return "sasl-encoding";
        case SaslInvalidAuthzid:
            return "sasl-invalid-authzid";
        case SaslInvalidMechanism:
            return "sasl-invalid-mechanism";
        case SaslMechanismTooWeak:
            return "sasl-too-weak";
        case SaslNotAuthorized:
            return "sasl-not-authorized";
        case SaslTemporaryAuthFailure:
            return "sasl-temporary-failure";
        case NonSaslConflict:
            return "nosasl-conflict";
        case NonSaslNotAcceptable:
            return "nosasl-not-acceptable";
        case NonSaslNotAuthorized:
            return "nosasl-not-authorized";
        default:
            return "unhandled";
    }
}

std::string EchoClient::authError()
{
    return authErrorToString(m_client->authError());
}

void EchoClient::sendChatMessage(const JID &to, const std::string &message)
{
    Stanza *msg = Stanza::createMessageStanza( to, message);
    m_client->send(msg);
}

