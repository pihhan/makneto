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
	bool logout();
	void setStatus(Status);
	bool isOnline();

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

signals:
	void connMessageReceived(const QString &s);
	void connConnected(void);
	void connDisconnected(void);

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
