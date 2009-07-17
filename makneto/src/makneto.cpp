/*
 * makneto.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "makneto.h"
#include "maknetocontactlist.h"
#include "connection.h"
#include "xmpp_tasks.h"

#include "contactdetaildialog.h"
#include "discorequest.h"

#include <iostream>

#include <QObject>

Makneto::Makneto(QObject *parent) : QObject(parent)
{
	// set up connection
	m_conn = new Connection(this);

	connect(m_conn, SIGNAL(connMessageReceived(const Message &)), this, SLOT(conn_messageReceived(const Message &)));
// 	connect(m_conn, SIGNAL(connStatusChanged(const Status &)), this, SLOT(conn_statusChanged(const Status &)));

	// create contact list
	m_cl = new MaknetoContactList(this);

        m_flm = new FeatureListManager(this);
        if (!m_flm->readDatabase()) {
            qWarning("Capabilities cache reading was unsuccessful");
        }
}

Makneto::~Makneto()
{
    if (m_flm) {
        delete m_flm;
    }
}

void Makneto::conn_messageReceived(const Message &)
{
	std::cout << "Makneto::conn_messageReceived" << std::endl;
}

void Makneto::actionNewSession(const QString &text, ChatType type, const QString &nick)
{
	std::cout << "Makneto::actionNewSession" << std::endl;
	
	emit newSession(text, type, nick);
}

void Makneto::actionNewSession()
{
	std::cout << "Makneto::actionNewSession-" << std::endl;
	
	emit newSession(getConnection()->jid().full(), Chat);
}

void Makneto::contactNewSession(QAction *action)
{
  emit newSession(action->data().toString(), Chat);
}

void Makneto::contactDetails(QAction *action)
{
  std::cout << "Makneto::contactDetail()" << std::endl;
  contactDetailDialog *contactDetail = new contactDetailDialog(m_mainwindow, action->data().toString());
  contactDetail->show();

  // pihhanovy hratky
  QString jid = action->data().toString();
  DiscoRequest *req = new DiscoRequest(m_conn->rootTask());
  req->get(jid, "");
  req->go(true);
  
}

void Makneto::addUser(const XMPP::Jid &jid, const QString &group, bool requestAuth)
{
	std::cout << "Makneto::addUser()" << std::endl;

	m_conn->addUser(jid, group, requestAuth);
	
}

#include "makneto.moc"

