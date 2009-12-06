
#include <iostream>
#include <gloox/logsink.h>
#include <gloox/jid.h>
#include <gloox/disco.h>
#include <gloox/rostermanager.h>
#include <gloox/rosteritem.h>
#ifdef DNS_RESOLVER
#include <gloox/dns.h>
#endif

#include "client.h"
#include "logger.h"
#include "parser.h"
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

    m_requests = new RequestList(this);
    m_verhandler = new VersionIqHandler(m_client);
    m_jingle = new JingleManager(m_client);
    if (m_jingle) {
        m_jingle->registerActionHandler(m_requests);
    }
}

void EchoClient::initLog()
{
    Logger::init();
    m_client->logInstance().registerLogHandler(LogLevelDebug, LogAreaAll, logger);
    //m_client->logInstance().registerLogHandler(LogLevelWarning, LogAreaAll, logger);
    //m_client->logInstance().registerLogHandler(LogLevelError, LogAreaAll, logger);
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
        m_client->logInstance().log(LogLevelWarning, LogAreaUser, "Server certificate name and requested jid do not match.");
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

#ifdef DNS_RESOLVER
/** @brief Create string of host:port pairs that resolved from domain.
	@param domain DNS name to resolve.
	@param service Name of service for SRV lookup. If empty, only hostname is resolved. For example, "_xmpp-server"
	@param proto Protocol on which to lookup service. Only used if service is not empty.
	FIXME: DNS class is not present in this package
*/
std::string EchoClient::resolveToString(const std::string &domain, const std::string &service, std::string &proto)
{
			std::string reply;
			bool first = true;
			DNS::HostMap map;
			if (service.empty()) 
				map = DNS::resolve(domain, m_client->client()->logInstance());
			else
				map = DNS::resolve(service, proto, domain, m_client->client()->logInstance());
			
			if (map.size() == 0) {
				reply.append("(none)");
			} else {
				for (HostMap::iterator it = map.begin(); it != map.end(); it++) {
					if (!first)
						reply.append(", ");
					first = false;
					std::string hostport(it->first);
					if (!service.empty())
						hostport << ":" << it->second;
					reply.append(hostport);
				}
			}
			return reply;
}
#endif

void EchoClient::sendHelp(const Stanza *stanza)
{
    std::string helpmsg = 
        "Usage:\n"
        "disco [target jid]\n"
        "version [target jid]\n"
        "call [target]\n"
        "accept [sid]\n";
    sendChatMessage(stanza->from(), helpmsg);
}

void EchoClient::handleMessage (Stanza *stanza, MessageSession *session)
{
    std::string body = stanza->body();
    if (!body.empty()) {
        CL_DEBUG(m_client, "Message received: " + body);
        // m_client->send(bounceMessage(stanza));
        parser::Parser p(body);
        std::string reply("Parsed:");
        std::string cmd;
        std::string param;

        if (!p.atEnd()) {
            parser::Token t = p.nextToken();
            cmd = t.value();
        }
        if (!p.atEnd()) {
            param = p.nextToken().value();
        }
        if (cmd == "help" || cmd == "?") {
            sendHelp(stanza);
        } else if (cmd == "disco") {
            JID target(stanza->from());
            if (!param.empty()) {
                target = JID(param);
            }
            if (!target) {
                sendChatMessage(stanza->from(), "target is not valid jid: " + param);
                return;
            } else {
                m_requests->createDiscoRequest(stanza->from(), target);
            }
	} else if (cmd == "ver" || cmd == "version") {
			JID target(stanza->from());
			if (!param.empty())
				target = JID(param);
			if (!target) {
				sendChatMessage(stanza->from(), "target is not valid jid: " + param);
				return;
			} else {
				m_requests->createVersionRequest(stanza->from(), target);
			}
        } else if (cmd == "call" || cmd == "initate") {
            JID target(stanza->from());
            if (!param.empty())
                target = JID(param);

            JingleSession *session = 
                    m_jingle->initiateAudioSession(stanza->from(), target); 
            if (session) {
                std::string sid = session->sid();
                sendChatMessage(stanza->from(), " initiated session id:" + sid);
            } else {
                sendChatMessage(stanza->from(), "session is NULL");
            }
        } else if (cmd == "accept") {
            JingleSession *session = NULL;
            if (!param.empty()) {
                session = m_jingle->getSession(param);
            } else {
                JingleManager::SessionMap map = m_jingle->allSessions();
                JingleManager::SessionMap::iterator it = map.begin();
                if (it != map.end()) {
                    session = (*it).second;
                }
                sendChatMessage(stanza->from(), " found first session id:" 
                    + session->sid());
            }

            m_jingle->acceptAudioSession(session);
            sendChatMessage(stanza->from(), "session accepted");

#ifdef DNS_RESOVLER
	} else if (cmd == "dns") {
			if (param.empty()) {
				sendChatMessage(stanza->from(), "you have to specify name to resolve");
				return;
			}
			std::string reply;
			reply.append("XMPP server: ");
			reply.append(resolveToString(param, "_xmpp-server", "_tcp");
			reply.append("\n");
			reply.append("XMPP client: ");
			reply.append(resolveToString(param, "_xmpp-client", "_tcp");
			reply.append("\n");
			reply.append("Hostname: ");
			reply.append(resolveToString(param);
			m_client->sendChatMessage(stanza->from(), reply);
#endif				
        } else {
            while (!p.atEnd()) {
                parser::Token t = p.nextToken();
                reply.append("\"" + t.value() + "\" ");
            }
            sendChatMessage(stanza->from(), reply);
        }
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


std::string EchoClient::stanzaErrorToString(StanzaError e)
{
    switch (e) {
        case StanzaErrorUndefined:
            return "udefined";
        case StanzaErrorBadRequest:
            return "bad-request";
        case StanzaErrorConflict:
            return "conflict";
        case StanzaErrorFeatureNotImplemented:
            return "not-implemented";
        case StanzaErrorForbidden:
            return "forbidden";
        case StanzaErrorGone:
            return "gone";
        case StanzaErrorInternalServerError:
            return "internal-server-error";
        case StanzaErrorItemNotFound:
            return "item-not-found";
        case StanzaErrorJidMalformed:
            return "jid-malformed";
        case StanzaErrorNotAcceptable:
            return "not-acceptable";
        case StanzaErrorNotAllowed:
            return "not-allowed";
        case StanzaErrorNotAuthorized:
            return "not-authorized";
        case StanzaErrorPaymentRequired:
            return "payment-required";
        case StanzaErrorRecipientUnavailable:
            return "recipient-unavailable";
        case StanzaErrorRedirect:
            return "redirect";
        case StanzaErrorRegistrationRequired:
            return "registration-required";
        case StanzaErrorRemoteServerNotFound:
            return "remote-server-not-found";
        case StanzaErrorRemoteServerTimeout:
            return "remote-server-timeout";
        case StanzaErrorResourceConstraint:
            return "resource-contraint";
        case StanzaErrorServiceUnavailable:
            return "service-unavailable";
        case StanzaErrorSubscribtionRequired:
            return "subscription-required";
        case StanzaErrorUndefinedCondition:
            return "undefined-condition";
        case StanzaErrorUnexpectedRequest:
            return "unexpected-request";
        default:
            return "unhandled";
    }
}

/** @brief Create description of error from stanza */
std::string EchoClient::describeError(const Stanza *stanza)
{
	std::string description;
	description = stanzaErrorToString(stanza->error());
	if (!stanza->errorText().empty()) {
		description.append(": " + stanza->errorText().empty());
	}
	return description;
}

VersionIqHandler *EchoClient::versionHandler()
{
	return m_verhandler;
}


/** @brief Return list of online JIDs from roster.
	@param resources if true, enumerate resources and fill full jids for each contact, \
	with all online resources for each contact. if false, include only bare jids.
*/
JidList	EchoClient::getOnlineJids(bool resources)
{
	JidList list;
	Roster *roster = m_client->rosterManager()->roster();
	if (!roster)
		return list;
	for (Roster::iterator it=roster->begin(); it!=roster->end(); it++) {
		RosterItem *item = it->second;
		if (item) {
			if (item->online() && !resources) 
				list.push_back(item->jid());
			else if (resources) {
				JID jid(item->jid());
				for (RosterItem::ResourceMap::const_iterator it=item->resources().begin(); it!=item->resources().end(); it++) {
					jid.setResource(it->first);
					list.push_back(jid);
				}
			}
		}
	}
	return list;
}


bool EchoClient::isAdmin(const JID &jid)
{
	const std::string admingroup("admin");
	RosterItem *item = m_client->rosterManager()->getRosterItem(jid);
	if (!jid) // not in roster
		return false;
	else {
		StringList groups = item->groups();
		for (StringList::iterator it=groups.begin(); it!=groups.end(); it++) {
			if ((*it) == admingroup)
				return true; // is in admin group
		}
		return false; // is in roster, but not in admin group
	}
}

/** @brief Create message and send it to all online contacts. */
void EchoClient::broadcastChatMessage(const std::string &message)
{
	JidList online = getOnlineJids(true);
	for (JidList::iterator it=online.begin(); it!=online.end(); it++) {
		sendChatMessage(*it, message);
	}
}
