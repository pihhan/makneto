/*
 * makneto.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "makneto.h"
#include "maknetocontactlist.h"
#include "connection.h"

#include <iostream>

#include <QObject>

Makneto::Makneto(QObject *parent) : QObject(parent)
{
	// set up connection
	m_conn = new Connection(this);

	connect(m_conn, SIGNAL(connMessageReceived(const Message &)), this, SLOT(conn_messageReceived(const Message &)));

	// create contact list
	m_cl = new MaknetoContactList(this);
}

Makneto::~Makneto()
{

}

void Makneto::conn_messageReceived(const Message &)
{
	std::cout << "Makneto::conn_messageReceived" << std::endl;
}

void Makneto::actionNewSession(const QString &text)
{
	std::cout << "Makneto::actionNewSession" << std::endl;
	
	emit newSession(text);
}

void Makneto::actionNewSession()
{
	std::cout << "Makneto::actionNewSession-" << std::endl;
	
	emit newSession("rezzabuh@jabber.cz/Makneto");
}

void Makneto::contactTriggered(QAction *action)
{
	emit newSession(action->data().toString() + "/Makneto");
}

#include "makneto.moc"

