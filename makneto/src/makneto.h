/*
 * makneto.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETO_H
#define MAKNETO_H

#include "maknetomainwindow.h"
#include "connection.h"

#include <QObject>

namespace XMPP 
{
	class Message;
	class Status;
}

using namespace XMPP;

/**
 * This is Makneto class - central class of Makneto suite
 *
 * @short Makneto
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class MaknetoContactList;

enum ChatType { Chat = 0, GroupChat };

class Makneto : public QObject
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	Makneto(QObject *parent=0);

	/**
	* Destructor
	*/
	virtual ~Makneto();

	Connection *getConnection() { return m_conn; }
	MaknetoContactList *getContactList() { return m_cl; }

signals:
	void newSession(const QString &text, ChatType type);
	void statusChanged(const XMPP::Status &);

public slots:
	void conn_messageReceived(const Message &);
// 	void conn_statusChanged(const XMPP::Status &);
  void actionNewSession(const QString &, ChatType type = Chat);
	void actionNewSession();
	void contactTriggered(QAction *action);
	void addUser(const XMPP::Jid &, const QString &, bool requestAuth);

private:
	Connection *m_conn;
	MaknetoContactList *m_cl;
};

#endif // MAKNETO_H
