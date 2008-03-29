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

public slots:
	void conn_messageReceived(const QString &);

private:
	Connection *m_conn;
	MaknetoContactList *m_cl;
};

#endif // MAKNETO_H
