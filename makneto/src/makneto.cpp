/*
 * makneto.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "makneto.h"
#include "maknetocontactlist.h"
#include "connection.h"
#include <xmpp_tasks.h>
#include <xmpp_jid.h>

#include "contactdetaildialog.h"
#include "discorequest.h"
#include "mediamanager.h"

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

        m_mediamgr = new MediaManager(this);
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
  XMPP::Jid target(action->data().toString());
  contactDetailDialog *contactDetail = new contactDetailDialog(m_mainwindow, action->data().toString());
  contactDetail->show();

  // pihhanovy hratky
  DiscoRequest *req = new DiscoRequest(m_conn->rootTask());
  req->get(target.full(), "");
  req->go(true);

  // get info
  Connection *conn = getConnection();
  if (conn && target.isValid()) {
    XMPP::JT_VCard *vcard_req = new XMPP::JT_VCard(conn->rootTask());
    vcard_req->get(target);
    connect(vcard_req, SIGNAL(finished()), 
            contactDetail, SLOT(detailsArrived()) );
    vcard_req->go(false);

    contactDetail->describeContact(getContactList()->getContact(target.bare()));
  }
  
}

void Makneto::addUser(const XMPP::Jid &jid, const QString &group, bool requestAuth)
{
	std::cout << "Makneto::addUser()" << std::endl;

	m_conn->addUser(jid, group, requestAuth);
	
}

MUCControl * Makneto::getMUCControl() const
{
    return m_muccontrol;
}

void Makneto::setMUCControl(MUCControl *muc)
{
    m_muccontrol = muc;
}

MediaManager * Makneto::getMediaManager() const
{
    return m_mediamgr;
}

#include "makneto.moc"

