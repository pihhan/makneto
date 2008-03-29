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

	connect(m_conn, SIGNAL(connMessageReceived(const QString &)), this, SLOT(conn_messageReceived(const QString &)));

	// create contact list
	m_cl = new MaknetoContactList();
}

Makneto::~Makneto()
{

}

void Makneto::conn_messageReceived(const QString &)
{
	std::cout << "Makneto::conn_messageReceived" << std::endl;
}

#include "makneto.moc"

