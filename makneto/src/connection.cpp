/*
 * connection.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "connection.h"

#include "xmpp.h"
#include "xmpp_client.h"
#include "xmpp_clientstream.h"
#include "xmpp_jid.h"
#include "xmpp_tasks.h"
#include "xmpp_features.h"

#include <QtCrypto>

using namespace XMPP;

Connection::Connection()
{
	m_tlsHandler = 0;
	m_conn = 0;
	m_stream = 0;
	m_tls = 0;
	m_jid = 0;

	m_client = new Client();
	m_client->setClientName("Makneto");

	QStringList features;
	features << "http://jabber.org/protocol/commands";
	features << "http://jabber.org/protocol/rosterx";
	m_client->setFeatures(Features(features));

	connect(m_client, SIGNAL(debugText(const QString &)), SLOT(client_debugText(const QString &)));
	connect(m_client, SIGNAL(messageReceived(const Message &)), this, SLOT(client_messageReceived(const Message &)));
	connect(m_client, SIGNAL(rosterRequestFinished(bool, int, const QString &)), this, SLOT(client_rosterRequestFinished(bool, int, const QString &)));
	connect(m_client, SIGNAL(rosterItemAdded(const RosterItem &)), SLOT(client_rosterItemAdded(const RosterItem &)));
	connect(m_client, SIGNAL(rosterItemUpdated(const RosterItem &)), SLOT(client_rosterItemUpdated(const RosterItem &)));
	connect(m_client, SIGNAL(rosterItemRemoved(const RosterItem &)), SLOT(client_rosterItemRemoved(const RosterItem &)));
	connect(m_client, SIGNAL(resourceAvailable(const Jid &, const Resource &)), SLOT(client_resourceAvailable(const Jid &, const Resource &)));
	connect(m_client, SIGNAL(resourceUnavailable(const Jid &, const Resource &)), SLOT(client_resourceUnavailable(const Jid &, const Resource &)));
	connect(m_client, SIGNAL(presenceError(const Jid &, int, const QString &)), SLOT(client_presenceError(const Jid &, int, const QString &)));
	connect(m_client, SIGNAL(subscription(const Jid &, const QString &, const QString&)), SLOT(client_subscription(const Jid &, const QString &, const QString&)));
	connect(m_client, SIGNAL(xmlIncoming(const QString &)), this, SLOT(client_xmlIncoming(const QString &)));
	connect(m_client, SIGNAL(xmlOutgoing(const QString &)), this, SLOT(client_xmlOutgoing(const QString &)));
}

Connection::~Connection()
{

}

bool Connection::login()
{
	qDebug() << "Connection::connect()";

	m_jid = Jid("rezza@jabber.cz");

	m_conn = new AdvancedConnector(this);
	m_conn->setOptSSL(false);
	m_conn->setOptProbe(false);

	m_stream = new ClientStream(m_conn, m_tlsHandler);
	m_stream->setAllowPlain(ClientStream::AllowPlain);

	connect(m_stream, SIGNAL(needAuthParams(bool, bool, bool)), SLOT(needAuthParams(bool, bool, bool)));
	connect(m_stream, SIGNAL(connected()), SLOT(connected()));
	connect(m_stream, SIGNAL(connectionClosed()), SLOT(connectionClosed()));
	connect(m_stream, SIGNAL(warning(int)), SLOT(warning(int)));
	connect(m_stream, SIGNAL(error(int)), SLOT(error(int)));
	connect(m_stream, SIGNAL(authenticated()), SLOT(authenticated()));

	m_client->connectToServer(m_stream, m_jid, true);

	return true;
}

bool Connection::logout()
{
	m_client->close();

	return true;
}

void Connection::needAuthParams(bool, bool, bool)
{
	qDebug() << "Connection::needAuthParams(...)";

	m_stream->setUsername("rezza");
	m_stream->setPassword("Pondeli1");

	m_stream->continueAfterParams();
}

void Connection::connected()
{
	qDebug() << "Connection::connected()";
}

void Connection::connectionClosed()
{
	qDebug() << "Connection::connectionClosed()";
}

void Connection::warning(int code)
{
	qDebug() << "Connection::warning(" << code << ")";

	if (code == ClientStream::WarnOldVersion)
		qDebug() << "Connection: server uses older XMPP version";
	else if (code == ClientStream::WarnNoTLS)
		qDebug() << "Connection: no TLS";

	// pass warning
	m_stream->continueAfterWarning();
}

void Connection::error(int code)
{
	qDebug() << "Connection::error(" << code << ")";

	QString str;
	bool rem_conn;

	if(code == -1) {
		str = tr("Dism_connected");
		rem_conn = true;
	}
	else if(code == ClientStream::ErrParse) {
		str = tr("XML Parsing Error");
		rem_conn = true;
	}
	else if(code == ClientStream::ErrProtocol) {
		str = tr("XMPP Protocol Error");
		rem_conn = true;
	}
	else if(code == ClientStream::ErrStream) {
		int x;
		QString s;
		rem_conn = true;
		if (m_stream) // Stream can apparently be gone if you dism_connect in time
			x = m_stream->errorCondition();
		else {
			x = Stream::GenericStreamError;
			rem_conn = false;
		}

		if(x == Stream::GenericStreamError)
			s = tr("Generic m_stream error");
		else if(x == ClientStream::Conflict) {
			s = tr("Conflict (remote login replacing this one)");
			rem_conn = false;
		}
		else if(x == ClientStream::ConnectionTimeout)
			s = tr("Timed out from inactivity");
		else if(x == ClientStream::InternalServerError)
			s = tr("Internal server error");
		else if(x == ClientStream::InvalidXml)
			s = tr("Invalid XML");
		else if(x == ClientStream::PolicyViolation) {
			s = tr("Policy violation");
			rem_conn = false;
		}
		else if(x == ClientStream::ResourceConstraint) {
			s = tr("Server out of resources");
			rem_conn = false;
		}
		else if(x == ClientStream::SystemShutdown)
			s = tr("Server is shutting down");
		str = tr("XMPP Stream Error: %1").arg(s);
	}
	else if(code == ClientStream::ErrConnection) {
		int x = m_conn->errorCode();
		QString s;
		rem_conn = true;
		if(x == AdvancedConnector::ErrConnectionRefused)
			s = tr("Unable to m_connect to server");
		else if(x == AdvancedConnector::ErrHostNotFound)
			s = tr("Host not found");
		else if(x == AdvancedConnector::ErrProxyConnect)
			s = tr("Error m_connecting to proxy");
		else if(x == AdvancedConnector::ErrProxyNeg)
			s = tr("Error during proxy negotiation");
		else if(x == AdvancedConnector::ErrProxyAuth) {
			s = tr("Proxy authentication failed");
			rem_conn = false;
		}
		else if(x == AdvancedConnector::ErrStream)
			s = tr("Socket/m_stream error");
		str = tr("Connection Error: %1").arg(s);
	}
	else if(code == ClientStream::ErrNeg) {
		int x = m_stream->errorCondition();
		QString s;
		if(x == ClientStream::HostGone)
			s = tr("Host no longer hosted");
		else if(x == ClientStream::HostUnknown)
			s = tr("Host unknown");
		else if(x == ClientStream::RemoteConnectionFailed) {
			s = tr("A required remote m_connection failed");
			rem_conn = true;
		}
		else if(x == ClientStream::SeeOtherHost)
			s = tr("See other host: %1").arg(m_stream->errorText());
		else if(x == ClientStream::UnsupportedVersion)
			s = tr("Server does not support proper XMPP version");
		str = tr("Stream Negotiation Error: %1").arg(s);
	}
	else if(code == ClientStream::ErrTLS) {
		int x = m_stream->errorCondition();
		QString s;
		if(x == ClientStream::TLSStart)
			s = tr("Server rejected STARTTLS");
		else if(x == ClientStream::TLSFail) {
			int t = m_tlsHandler->tlsError();
			if(t == QCA::TLS::ErrorHandshake)
				s = tr("TLS handshake error");
			else
				s = tr("Broken security layer (TLS)");
		}
		str = s;
	}
	else if(code == ClientStream::ErrAuth) {
		int x = m_stream->errorCondition();
		QString s;
		if(x == ClientStream::GenericAuthError)
			s = tr("Unable to login");
		else if(x == ClientStream::NoMech)
			s = tr("No appropriate mechanism available for given security settings (e.g. SASL library too weak, or plaintext authentication not enabled)");
		else if(x == ClientStream::BadProto)
			s = tr("Bad server response");
		else if(x == ClientStream::BadServ)
			s = tr("Server failed mutual authentication");
		else if(x == ClientStream::EncryptionRequired)
			s = tr("Encryption required for chosen SASL mechanism");
		else if(x == ClientStream::InvalidAuthzid)
			s = tr("Invalid account information");
		else if(x == ClientStream::InvalidMech)
			s = tr("Invalid SASL mechanism");
		else if(x == ClientStream::InvalidRealm)
			s = tr("Invalid realm");
		else if(x == ClientStream::MechTooWeak)
			s = tr("SASL mechanism too weak for this account");
		else if(x == ClientStream::NotAuthorized)
			s = tr("Not authorized");
		else if(x == ClientStream::TemporaryAuthFailure)
			s = tr("Temporary auth failure");
		str = tr("Authentication error: %1").arg(s);
	}
	else if(code == ClientStream::ErrSecurityLayer)
		str = tr("Broken security layer (SASL)");
	else
		str = tr("None");

	qDebug() << str;
}

void Connection::authenticated()
{
	qDebug() << "Connection::authenticated()";

	qDebug() << m_jid.host() << "," << m_jid.user();

	m_client->start(m_jid.host(), m_jid.user(), "test", "makneto");

	if (!m_stream->old())
	{
		JT_Session *j = new JT_Session(m_client->rootTask());
		connect(j,SIGNAL(finished()),SLOT(sessionStart_finished()));
		j->go(true);
	}
	else
	{
		sessionStarted();
	}
}

void Connection::sessionStart_finished()
{
	JT_Session *j = (JT_Session*)sender();
	if ( j->success() ) 
	{
		Status s;
		s.setIsAvailable(true);
		s.setType(Status::Online);
		m_client->setPresence(s);

		sessionStarted();
	}
	else {
		error(-1);
	}
}

void Connection::tlsHandshaken()
{
	qDebug() << "Connection::tlsHandshaken()";
}

void Connection::sessionStarted()
{
	qDebug() << "Connection::sessionStarted()";

	m_client->rosterRequest();

}

void Connection::setStatus(Status status)
{
	m_client->setPresence(status);
}

bool Connection::isOnline()
{
	if(m_stream)
		return m_stream->isAuthenticated();
}

void Connection::client_rosterRequestFinished(bool, int, const QString &)
{
	qDebug() << "Connection::client_rosterRequestFinished()";
}

void Connection::client_rosterItemAdded(const RosterItem &item)
{
	qDebug() << "Connection::client_rosterItemAdded(item)";

	qDebug() << item.name();
	qDebug() << item.groups();
}

void Connection::client_rosterItemUpdated(const RosterItem &)
{
	qDebug() << "Connection::client_rosterItemUpdated(item)";
}

void Connection::client_rosterItemRemoved(const RosterItem &)
{
	qDebug() << "Connection::client_rosterItemRemoved(item)";
}

void Connection::client_resourceAvailable(const Jid &, const Resource &)
{
	qDebug() << "Connection::client_resourceAvailable()";
}

void Connection::client_resourceUnavailable(const Jid &, const Resource &)
{
	qDebug() << "Connection::client_resourceUnavailable()";
}

void Connection::client_presenceError(const Jid &, int, const QString &)
{
	qDebug() << "Connection::client_presenceError()";
}

void Connection::client_messageReceived(const Message &message)
{
	qDebug() << "Connection::client_messageReceived()";

	qDebug() << message.body();
}

void Connection::client_subscription(const Jid &, const QString &, const QString&)
{
}

void Connection::client_debugText(const QString &debugText)
{
	qDebug() << debugText;	
}

void Connection::client_xmlIncoming(const QString &)
{
}

void Connection::client_xmlOutgoing(const QString &)
{
}

#include "connection.moc"
