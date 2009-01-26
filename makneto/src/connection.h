/*
 * connection.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

#include "xmpp_status.h"
#include "xmpp_rosterx.h"
#include "xmpp_jid.h"

/**
 * This is main connection class for Makneto
 *
 * @short Connection class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

namespace XMPP 
{
	class Client;
	class Jid;
	class ClientStream;
	class AdvancedConnector;
	class QCATLSHandler;
	class RosterItem;
	class Resource;
	class Message;
	class FileTransfer;
	class Status;
}

namespace QCA 
{
	class TLS;
}

using namespace XMPP;

class Makneto;

class Connection : public QObject
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	Connection(Makneto *makneto);

	/**
	* Destructor
	*/
	virtual ~Connection();

	bool login();
	void clientDisconnect();
	bool logout();
	void setStatus(Status);
	bool isOnline();
  bool groupChatJoin(const QString &host, const QString &room, const QString &nick, const QString& password = QString(), int maxchars = -1, int maxstanzas = -1, int seconds = -1, const Status& = Status());

private slots:
	void connected();
	void needAuthParams(bool, bool, bool);
	void warning(int code);
	void error(int code);
	void authenticated();
	void connectionClosed();
	void tlsHandshaken();
	void sessionStart_finished();

	void client_rosterRequestFinished(bool, int, const QString &);
	void client_rosterItemAdded(const RosterItem &);
	void client_rosterItemUpdated(const RosterItem &);
	void client_rosterItemRemoved(const RosterItem &);
	void client_resourceAvailable(const Jid &, const Resource &);
	void client_resourceUnavailable(const Jid &, const Resource &);
	void client_presenceError(const Jid &, int, const QString &);
	void client_messageReceived(const Message &message);
	void client_subscription(const Jid &, const QString &, const QString&);
	void client_debugText(const QString &);
	void client_xmlIncoming(const QString &);
	void client_xmlOutgoing(const QString &);
	void client_incomingFileTransfer();
  void client_groupChatJoined(const Jid &);
  void client_groupChatLeft(const Jid &);
  void client_groupChatPresence(const Jid &, const Status &);
  void client_groupChatError(const Jid &, int, const QString &);

	void sendMessage(const Message &);
public slots:
	void addUser(const Jid &jid, const QString &group, bool requestAuth);

signals:
	void connMessageReceived(const Message &);
	void connConnected(void);
	void connDisconnected(void);
	void connIncomingFileTransfer(FileTransfer *);
	void connStatusChanged(const XMPP::Status &);
  void groupChatJoined(const Jid &);
  void groupChatLeft(void);
  void groupChatPresence(const Status &);
  void groupChatError(const QString &);
  

private:
	Client *m_client;
	ClientStream *m_stream;
	AdvancedConnector *m_conn;
	QCATLSHandler *m_tlsHandler;
	Jid m_jid;
	QCA::TLS *m_tls;
	Makneto *m_makneto;
	bool m_rosterFinished;

protected:
	void sessionStarted();
};

#endif // CONNECTION_H
