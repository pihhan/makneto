/*
 * connectionview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "connectionview.h"
#include "kiconloader.h"
#include "klocale.h"
#include "connection.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include "xmpp_status.h"

ConnectionView::ConnectionView(QWidget *)
{
	m_buttonslayout = new QVBoxLayout(this);
	m_buttonslayout->setMargin(0);
	m_buttonslayout->setSpacing(0);

	m_buttononline = new QPushButton(KIconLoader::global()->loadIcon("hi16-action-jabber_online", KIconLoader::Toolbar, KIconLoader:: SizeSmall), i18n("&Online"), this);
	m_buttonslayout->addWidget(m_buttononline);
	connect(m_buttononline, SIGNAL(clicked(bool)), SLOT(onlineClicked(bool)));

	m_buttonaway = new QPushButton("&Away", this);
	m_buttonslayout->addWidget(m_buttonaway);
	connect(m_buttonaway, SIGNAL(clicked(bool)), SLOT(awayClicked(bool)));

	m_buttonxa = new QPushButton("&Extended Away", this);
	m_buttonslayout->addWidget(m_buttonxa);
	connect(m_buttonxa, SIGNAL(clicked(bool)), SLOT(xaClicked(bool)));

	m_buttondnd = new QPushButton("&Do not disturb", this);
	m_buttonslayout->addWidget(m_buttondnd);
	connect(m_buttondnd, SIGNAL(clicked(bool)), SLOT(dndClicked(bool)));

	m_buttoninvisible = new QPushButton("&Invisible", this);
	m_buttonslayout->addWidget(m_buttoninvisible);
	connect(m_buttoninvisible, SIGNAL(clicked(bool)), SLOT(invisibleClicked(bool)));

	m_buttonoffline = new QPushButton("O&ffline", this);
	m_buttonslayout->addWidget(m_buttonoffline);
	connect(m_buttonoffline, SIGNAL(clicked(bool)), SLOT(offlineClicked(bool)));

	setLayout(m_buttonslayout);

	m_conn = new Connection();
}

ConnectionView::~ConnectionView()
{

}

void ConnectionView::onlineClicked(bool)
{
	// check if we are yet logged in
	if(!m_conn->isOnline())
		m_conn->login();

	// otherwise just set status to Online
	m_conn->setStatus(XMPP::Status::Online);
}

void ConnectionView::awayClicked(bool)
{
	// check if we are yet logged in
	if(!m_conn->isOnline())
		m_conn->login();

	// set status to Away
	m_conn->setStatus(XMPP::Status::Away);
}

void ConnectionView::xaClicked(bool)
{
	// check if we are yet logged in
	if(!m_conn->isOnline())
		m_conn->login();

	// set status to XA
	m_conn->setStatus(XMPP::Status::XA);
}

void ConnectionView::dndClicked(bool)
{
	// check if we are yet logged in
	if(!m_conn->isOnline())
		m_conn->login();

	// set status to DND
	m_conn->setStatus(XMPP::Status::DND);
}

void ConnectionView::invisibleClicked(bool)
{
	// check if we are yet logged in
	if(!m_conn->isOnline())
		m_conn->login();

	// set status to Invisible
	m_conn->setStatus(XMPP::Status::Invisible);
}

void ConnectionView::offlineClicked(bool)
{
	if(m_conn->isOnline())
		m_conn->logout();
}
