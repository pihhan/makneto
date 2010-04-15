/*
 * connection.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "connection.h"
#include "makneto.h"
#include "maknetocontactlist.h"

#include "xmpp.h"
#include "xmpp_client.h"
#include "xmpp_clientstream.h"
#include "xmpp_jid.h"
#include "xmpp_tasks.h"
#include "xmpp_features.h"
#include "filetransfer.h"
#include "xmpp_tasks.h"
#include <xmpp_discoitem.h>

#include "featurelist.h"


#include "settings.h"

#include <QtCrypto>
#include <QMessageBox>

using namespace XMPP;

Connection::Connection(Makneto *makneto): m_makneto(makneto)
{

	m_tlsHandler = 0;
	m_conn = 0;
	m_stream = 0;
	m_tls = 0;
	m_jid = 0;

	m_rosterFinished = false;

	m_client = new Client();
	m_client->setClientName("Makneto");
        DiscoItem::Identity identity;
        identity.category = "client";
        identity.type = "pc";
        identity.name = "Makneto";
        m_client->setIdentity(identity);
        

        // FIXME: make this better and more secure, also this does memleak
        m_tls = new QCA::TLS();
        //m_tlsHandler = new XMPP::QCATLSHandler(m_tls);

	QStringList features;
        // FIXME: this features list is invalid
        features << "http://jabber.org/protocol/commands" << "http://www.w3.org/2000/svg";
        features << "http://jabber.org/protocol/svgwb";
        features << "urn:xmpp:jingle:1";
        features << "http://jabber.org/protocol/disco#info";
        features << "http://jabber.org/protocol/disco#items";
        features << "urn:xmpp:jingle:apps:rtp:1";
	m_client->setFeatures(Features(features));

	m_client->setFileTransferEnabled(true);

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
	connect(m_client->fileTransferManager(), SIGNAL(incomingReady()), SLOT(client_incomingFileTransfer()));
  connect(m_client, SIGNAL(groupChatJoined(const Jid &)), SLOT(client_groupChatJoined(const Jid &)));
  connect(m_client, SIGNAL(groupChatLeft(const Jid &)), SLOT(client_groupChatLeft(const Jid &)));
  connect(m_client, SIGNAL(groupChatPresence(const Jid &, const Status &)), SLOT(client_groupChatPresence(const Jid &, const Status &)));
  connect(m_client, SIGNAL(groupChatError(const Jid &, int, const QString &)), SLOT(client_groupChatError(const Jid &, int, const QString &)));
  
  
  // TODO: Remove logging when not necessary
  QFile *logFile = new QFile("message.log");
  logFile->open(QIODevice::WriteOnly);
  log.setDevice(logFile);
}

Connection::~Connection()
{
  // TODO: Remove logging when not necessary
  log.flush();
  log.device()->close();
  
	logout();


	delete m_stream;
	m_stream = 0;

	delete m_conn;
	m_conn = 0;

	delete m_client;
	m_client = 0;

}

bool Connection::login()
{
	qDebug() << "Connection::connect()";

	// my jabber id from settings
	m_jid = Jid(Settings::jabberID());
        if (m_jid.resource().isEmpty()) {
	        m_jid = m_jid.withResource("Makneto");
        }

	m_conn = new AdvancedConnector(this);

	// TODO:ssl connection
	m_conn->setOptSSL(false);
	m_conn->setOptProbe(false);
	
	if (!Settings::jabberHost().isEmpty())
	{
		qDebug() << "Using host settings";
		m_conn->setOptHostPort(Settings::jabberHost(), Settings::jabberPort());
	}

	m_stream = new ClientStream(m_conn, m_tlsHandler);

	if (Settings::allowPlain())
		m_stream->setAllowPlain(ClientStream::AllowPlain);

        // this makes problems with integrated Simple SASL library.
	m_stream->setRequireMutualAuth(false);

	connect(m_stream, SIGNAL(needAuthParams(bool, bool, bool)), SLOT(needAuthParams(bool, bool, bool)));
	connect(m_stream, SIGNAL(connected()), SLOT(connected()));
	connect(m_stream, SIGNAL(connectionClosed()), SLOT(connectionClosed()));
	connect(m_stream, SIGNAL(warning(int)), SLOT(warning(int)));
	connect(m_stream, SIGNAL(error(int)), SLOT(error(int)));
	connect(m_stream, SIGNAL(authenticated()), SLOT(authenticated()));

	Jid j = m_jid;

	m_client->connectToServer(m_stream, j, true);

	return true;
}

bool Connection::logout()
{
	setStatus(XMPP::Status::Offline);

	m_client->close();

	delete m_stream;
	m_stream = 0;

	delete m_conn;
	m_conn = 0;

	return true;
}

void Connection::clientDisconnect()
{
	logout();

	m_client->close();
}

void Connection::needAuthParams(bool, bool, bool)
{
	qDebug() << "Connection::needAuthParams(...)";

	m_stream->setUsername(m_jid.node());
	m_stream->setPassword(Settings::jabberPassword());

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

	QMessageBox::critical(0, tr("Makneto"),
                   str,
                   QMessageBox::Ok,
                   QMessageBox::Ok);

	qDebug() << str;
}

void Connection::authenticated()
{
	qDebug() << "Connection::authenticated()";

	m_client->start(m_jid.domain(), m_jid.node(), "test", m_jid.resource());

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
// 		Status s;
// 		s.setIsAvailable(true);
// 		s.setType(Status::Online);
// 		m_client->setPresence(s);

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
    QString hashable = FeatureList::computeHashString(m_client->identity(), m_client->features());
    QString sha1hash = FeatureListManager::getCryptoSHA1(hashable);
    qDebug() << "Client has capabilities hashable: " << hashable 
        << " with hash: " << sha1hash;
    status.setCapsVersion(sha1hash);
    status.setCapsNode("Makneto");

	if (m_rosterFinished) 
		m_client->setPresence(status);


	emit connStatusChanged(status);
}

bool Connection::isOnline()
{
	if(m_stream)
		return m_stream->isAuthenticated();
        else
                return false;
}

void Connection::client_rosterRequestFinished(bool success, int, const QString &)
{
        Q_UNUSED(success)
	qDebug() << "Connection::client_rosterRequestFinished()";

	m_rosterFinished = true;

	// status change after roster request finished! otherwise we lost presence
	// from contacts not in roster!!!
	setStatus(XMPP::Status::Online);
}

bool Connection::groupChatJoin(const QString &host, const QString &room, const QString &nick, const QString& password, int maxchars, int maxstanzas, int seconds, const Status& status)
  {
  return m_client->groupChatJoin(host, room, nick, password, maxchars, maxstanzas, seconds, status);
}

void Connection::groupChatLeave(const Jid &jid)
{
  m_client->groupChatLeave(jid.domain(), jid.node());
}

void Connection::setRole(const Jid &jid, const QString &nick, const QString &role, const QString &reason)
{
  QString message;
  message = QString(
"<iq from='%1' id='%2' to='%3' type='set'>\n\
  <query xmlns='http://jabber.org/protocol/muc#admin'>\n\
    <item nick='%4' role='%5'").arg(m_jid.full(), m_client->genUniqueId(), jid.bare(), nick, role);
  if (reason.isEmpty())
    message += "/>\n";
  else
    message += QString(">\n      <reason>%1</reason>\n    </item>\n").arg(reason);
  message += "  </query>\n</iq>\n";
  send(message);

}

void Connection::setAffiliation(const Jid &jid, const Jid &userJid, const QString &affiliation, const QString &reason)
{
  QString message;
  message = QString(
"<iq from='%1' id='%2' to='%3' type='set'>\n\
  <query xmlns='http://jabber.org/protocol/muc#admin'>\n\
    <item affiliation='%4' jid='%5'").arg(m_jid.full(), m_client->genUniqueId(), jid.bare(), affiliation, userJid.bare());
  if (reason.isEmpty())
    message += "/>\n";
  else
    message += QString(">\n      <reason>%1</reason>\n    </item>\n").arg(reason);
  message += "  </query>\n</iq>\n";
  send(message);
}

QString Connection::genUniqueId()
{
  return m_client->genUniqueId();
}

void Connection::groupChatGrantVoice(const Jid &jid, const QString &nick, const QString &reason)
{
  setRole(jid, nick, "participant", reason);
}

void Connection::groupChatGrantModeration(const Jid &jid, const QString &nick, const QString &reason)
{
  setRole(jid, nick, "moderator", reason);
}

void Connection::groupChatGrantMembership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  setAffiliation(jid, userJid, "member", reason);
}

void Connection::groupChatGrantAdministration(const Jid &jid, const Jid &userJid, const QString &reason)
{
  setAffiliation(jid, userJid, "admin", reason);
}

void Connection::groupChatGrantOwnership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  setAffiliation(jid, userJid, "owner", reason);
}

void Connection::groupChatRevokeOwnership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  setAffiliation(jid, userJid, "admin", reason);
}

void Connection::groupChatRevokeAdministration(const Jid &jid, const Jid &userJid, const QString &reason)
{
  setAffiliation(jid, userJid, "member", reason);
}

void Connection::groupChatRevokeMembership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  setAffiliation(jid, userJid, "none", reason);
}

void Connection::groupChatRevokeModeration(const Jid &jid, const QString &nick, const QString &reason)
{
  setRole(jid, nick, "participant", reason);
}

void Connection::groupChatRevokeVoice(const Jid &jid, const QString &nick, const QString &reason)
{
  setRole(jid, nick, "visitor", reason);
}

void Connection::groupChatRequestVoice(const Jid &jid)
{
  QString message;
  message = QString(
"<message from='%1' to='%2'>\n\
  <x xmlns='jabber:x:data' type='submit'>\n\
    <field var='FORM_TYPE'>\n\
      <value>http://jabber.org/protocol/muc#request</value>\n\
    </field>\n\
    <field var='muc#role' type='text-single' label='Requested role'>\n\
      <value>participant</value>\n\
    </field>\n\
  </x>\n\
</message>\n").arg(m_jid.full(), jid.bare());
  send(message);
}

void Connection::groupChatSubjectChange(const Jid &jid, const QString &subject)
{
  QString message;
  message = QString(
"<message from='%1' to='%2' type='groupchat'>\n\
  <subject>%3</subject>\n\
</message>\n").arg(m_jid.full(), jid.bare(), subject);
  send(message);
}

void Connection::groupChatKickUser(const Jid &jid, const QString &nick, const QString &reason)
{
  setRole(jid, nick, "none", reason);
}

void Connection::groupChatBanUser(const Jid &jid, const Jid &userJid, const QString &reason)
{
  setAffiliation(jid, userJid, "outcast", reason);
}

void Connection::client_rosterItemAdded(const RosterItem &item)
{
	qDebug() << "Connection::client_rosterItemAdded(item)";

	if (item.groups().size()==0)
		m_makneto->getContactList()->addContact(item.name(), item.jid().full(), "");
	else
	{
		m_makneto->getContactList()->addContact(item.name(), item.jid().full(), item.groups());
	}

}

void Connection::client_rosterItemUpdated(const RosterItem &)
{
	qDebug() << "Connection::client_rosterItemUpdated(item)";
}

void Connection::client_rosterItemRemoved(const RosterItem &)
{
	qDebug() << "Connection::client_rosterItemRemoved(item)";
}

void Connection::client_resourceAvailable(const Jid &jid, const Resource &resource)
{
	qDebug() << "Connection::client_resourceAvailable()";
	m_makneto->getContactList()->setAvailability(jid.bare(), 
            resource.name(), resource.status());
}

void Connection::client_resourceUnavailable(const Jid &jid, const Resource &resource)
{
	qDebug() << "Connection::client_resourceUnavailable()";
	m_makneto->getContactList()->setAvailability(jid.bare(), 
                resource.name(), resource.status());
}

void Connection::client_presenceError(const Jid &, int, const QString &)
{
	qDebug() << "Connection::client_presenceError()";
}

void Connection::client_messageReceived(const Message &message)
{
	qDebug() << "Connection::client_messageReceived()";

	qDebug() << message.body();

	emit connMessageReceived(message);
}

void Connection::client_subscription(const Jid &, const QString &, const QString&)
{
}

void Connection::client_debugText(const QString &debugText)
{
	qDebug() << debugText;	
}

void Connection::client_xmlIncoming(const QString &text)
  {
  log << "----- INCOMING -----\n" << text << endl;
  // Here will be processed all messages that is not part of iris library (mainly MUC)
  QDomDocument doc;
  if (!doc.setContent(text, false))
    return;
  QDomElement docElem = doc.documentElement(), child;
  if (docElem.isNull())
    return;
  // <presence from="a@b/c" type="unavailable" to="x@y/z" >
  if (docElem.tagName().compare("presence") == 0)
  {
    QString from, to, type;
    from = docElem.attribute("from");
    to = docElem.attribute("to");
    type = docElem.attribute("type");
    
    // <x xmlns='http://jabber.org/protocol/muc#user'>
    child = docElem.firstChildElement("x");
    if (!child.isNull() && child.attribute("xmlns").compare("http://jabber.org/protocol/muc#user") == 0)
    {
      Jid jfrom(from);
      Jid bare(jfrom.bare());
      QString nick = jfrom.resource();
      QString actor, reason, affiliation, role, jid;
      child = docElem.firstChildElement("item");
      if (!child.isNull())
      {
        role = child.attribute("role");
        affiliation = child.attribute("affiliation");
        jid = child.attribute("jid");
        
        QDomElement child2 = child.firstChildElement("actor");
        if (!child2.isNull())
        {
          actor = child2.attribute("jid");
        }
        child2 = child.firstChildElement("reason");
        if (!child2.isNull())
        {
          reason = child2.text();
        }
        if (affiliation.compare("none") == 0)
        {
          Jid jid(Jid(from).bare());
          // deprecated, create bare jid using bare() method
//          jid.setResource(QString());
          Jid userJid(jid);
          emit groupChatMembershipRevoked(jid, userJid, reason);
        }
        else if (affiliation.compare("member") == 0)
        {
          emit groupChatMembershipGranted(bare, bare, reason);
        }
        if (role.compare("participant") == 0)
        {
          emit groupChatVoiceGranted(bare, nick, reason);
          return;
        }
        else if (role.compare("visitor") == 0)
        {
          emit groupChatVoiceRevoked(bare, nick, reason);
          return;
        }
      }
      child = docElem.firstChildElement("status");
      if (!child.isNull())
      {
        if (type.compare("unavailable") == 0)
        {
          QString nick = jfrom.resource();
          if (child.attribute("code").compare("307") == 0)
          {
            emit groupChatUserKicked(bare, nick, reason, actor);
            return;
          }
          else if (child.attribute("code").compare("301") == 0)
          {
            emit groupChatUserBanned(bare, nick, reason, actor);
            return;
          }
          else if (child.attribute("code").compare("301") == 0)
          {
            emit groupChatUserRemovedAsNonMember(bare, nick);
            return;
          }
        }
      }
    }
  }
  else if (docElem.tagName().compare("iq") == 0)
  {
    
  }
  else if (docElem.tagName().compare("message") == 0)
  {
    QString from, to;
    from = docElem.attribute("from");
    to = docElem.attribute("to");
    
    child = docElem.firstChildElement("subject");
    if (!child.isNull())
    {
      Jid jid(Jid(to).bare());
      emit groupChatSubjectChanged(jid, child.text());
    }
    
    child = docElem.firstChildElement("x");
    if (!child.isNull() && child.attribute("xmlns").compare("jabber:x:data") == 0 && child.attribute("type").compare("submit") == 0)
    {
      QString role, nick;
      QDomElement child2 = child.firstChildElement("field"), child3;
      while (!child2.isNull())
      {
        if (child2.attribute("var").compare("muc#role") == 0)
        {
          child3 = child2.firstChildElement("value");
          if (!child3.isNull())
            role = child3.text();
        }
        if (child2.attribute("var").compare("muc#roomnick") == 0)
        {
          child3 = child2.firstChildElement("value");
          if (!child3.isNull())
            nick = child3.text();
        }
        child2 = child2.nextSiblingElement("field");
      }
      if (!nick.isEmpty() && role.compare("participant") == 0)
      {
        Jid jfrom(from);
        QString nick = jfrom.resource();
        Jid jid(jfrom.bare());
        emit groupChatVoiceRequested(jid, nick);
        return;
      }
    }
  }
  }
  
void Connection::client_xmlOutgoing(const QString &text)
  {
  log << "----- OUTGOING -----\n" << text << endl;
  }


void Connection::client_incomingFileTransfer()
{
	qDebug() << "Connection::client_incomingFileTransfer()";

	FileTransfer *ft = m_client->fileTransferManager()->takeIncoming();

	if (!ft)
		return;

	// process incoming file transfer
	emit connIncomingFileTransfer(ft);
}

void Connection::client_groupChatJoined(const Jid &jid)
{
  qDebug() << "Connection::client_groupChatJoined(" << jid.bare() << ")";
  emit groupChatJoined(jid);

}

void Connection::client_groupChatLeft(const Jid &jid)
{
  qDebug() << "Connection::client_groupChatLeft(" << jid.bare() << ")";
  emit groupChatLeft(jid);
}

void Connection::client_groupChatPresence(const Jid &jid, const Status &status)
{
  qDebug() << "Connection::client_groupChatPresence(" << jid.bare() << ": " << status.status() << ")";
  emit groupChatPresence(jid, status);
}

void Connection::client_groupChatError(const Jid &jid, int i, const QString &error)
{
  qDebug() << "Connection::client_groupChatError(" << jid.bare() << ": " << error << ")";
  emit groupChatError(jid, i, error);
}


void Connection::sendMessage(const Message &message)
{
	m_client->sendMessage(message);
}

/*! \brief Add new jid to user's roster.
    \param jid Bare jid of person we are adding.
    \param group Name of group we want to add contact to.
    \param requestAuth should we request subscription from that contact also?
    \todo Support multiple groups per user on add.
    */
void Connection::addUser(const Jid &jid, const QString &group, bool requestAuth)
{
	JT_Roster *roster = new JT_Roster(m_client->rootTask());
	roster->set(jid, "", QStringList(group));
	roster->go(true);

	if (requestAuth)
		m_client->sendSubscription(jid, "subscribe");
}

void Connection::send(const QDomElement &e)
{
  m_client->send(e);
}

void Connection::send(const QString &s)
{
  m_client->send(s);
}


  /*! \brief Get Iris client connection */
Client * Connection::client() const
{ 
    return m_client; 
}

  /*! \brief Get root Task object to create new requests. */
Task * Connection::rootTask() const
{ 
    if (m_client) 
        return m_client->rootTask(); 
    else return NULL;
}


#include "connection.moc"
