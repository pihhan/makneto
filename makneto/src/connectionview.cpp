/*
 * connectionview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "connectionview.h"
#include <kiconloader.h>
#include <klocale.h>
#include "connection.h"
#include "makneto.h"
#include "settings.h"

#include <kicon.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include "xmpp_status.h"

ConnectionView::ConnectionView(QWidget *, Makneto *makneto): m_makneto(makneto)
{
	m_mainlayout = new QVBoxLayout(this);

	m_stateLayout = new QHBoxLayout();

	m_buttonslayout = new QVBoxLayout();
	m_buttonslayout->setMargin(0);
	m_buttonslayout->setSpacing(0);

	// user ID
	m_labelJabberID = new QLabel("<b>" + Settings::jabberID() + "</b>", this);
	m_labelJabberID->setAlignment(Qt::AlignHCenter);

	// user icon
	m_buttonUserIcon = new QPushButton;
	m_buttonUserIcon->setIconSize(QSize(64, 64));
	m_buttonUserIcon->setIcon(KIcon("maknetooffline"));

	m_stateLayout->addWidget(m_buttonUserIcon);

	m_labelStatus = new QLabel("<i>Offline</i>", this);
	m_stateLayout->addWidget(m_labelStatus);

	m_buttononline = new QPushButton(KIconLoader::global()->loadIcon("maknetoonline", KIconLoader::Toolbar), i18n("&Online"), this);
	m_buttonslayout->addWidget(m_buttononline);
	connect(m_buttononline, SIGNAL(clicked(bool)), SLOT(onlineClicked(bool)));

	m_buttonaway = new QPushButton(KIconLoader::global()->loadIcon("maknetoaway", KIconLoader::Toolbar), i18n("&Away"), this);
	m_buttonslayout->addWidget(m_buttonaway);
	connect(m_buttonaway, SIGNAL(clicked(bool)), SLOT(awayClicked(bool)));

	m_buttonxa = new QPushButton(KIconLoader::global()->loadIcon("maknetoxa", KIconLoader::Toolbar), i18n("&Extended Away"), this);
	m_buttonslayout->addWidget(m_buttonxa);
	connect(m_buttonxa, SIGNAL(clicked(bool)), SLOT(xaClicked(bool)));

	m_buttondnd = new QPushButton(KIconLoader::global()->loadIcon("maknetodnd", KIconLoader::Toolbar), i18n("&Do not disturb"), this);
	m_buttonslayout->addWidget(m_buttondnd);
	connect(m_buttondnd, SIGNAL(clicked(bool)), SLOT(dndClicked(bool)));

	m_buttoninvisible = new QPushButton(KIconLoader::global()->loadIcon("maknetoinvisible", KIconLoader::Toolbar), i18n("&Invisible"), this);
	m_buttonslayout->addWidget(m_buttoninvisible);
	connect(m_buttoninvisible, SIGNAL(clicked(bool)), SLOT(invisibleClicked(bool)));

	m_buttonoffline = new QPushButton(KIconLoader::global()->loadIcon("maknetooffline", KIconLoader::Toolbar), i18n("O&ffline"), this);
	m_buttonslayout->addWidget(m_buttonoffline);
	connect(m_buttonoffline, SIGNAL(clicked(bool)), SLOT(offlineClicked(bool)));

	connect(makneto->getConnection(), SIGNAL(connStatusChanged(const XMPP::Status &)), this, SLOT(statusChanged(const XMPP::Status &)));

	m_mainlayout->addWidget(m_labelJabberID);
	m_mainlayout->addLayout(m_stateLayout);
	m_mainlayout->addLayout(m_buttonslayout, Qt::AlignTop);
	m_mainlayout->addStretch();

	setLayout(m_mainlayout);
}

ConnectionView::~ConnectionView()
{

}

void ConnectionView::onlineClicked(bool)
{
	// check if we are yet logged in
	if(!m_makneto->getConnection()->isOnline())
	{
		m_makneto->getConnection()->login();
		
		return;
	}

	// otherwise just set status to Online
	m_makneto->getConnection()->setStatus(XMPP::Status::Online);
}

void ConnectionView::awayClicked(bool)
{
	// check if we are yet logged in
	if(!m_makneto->getConnection()->isOnline())
		m_makneto->getConnection()->login();

	// set status to Away
	m_makneto->getConnection()->setStatus(XMPP::Status::Away);
}

void ConnectionView::xaClicked(bool)
{
	// check if we are yet logged in
	if(!m_makneto->getConnection()->isOnline())
		m_makneto->getConnection()->login();

	// set status to XA
	m_makneto->getConnection()->setStatus(XMPP::Status::XA);
}

void ConnectionView::dndClicked(bool)
{
	// check if we are yet logged in
	if(!m_makneto->getConnection()->isOnline())
		m_makneto->getConnection()->login();

	// set status to DND
	m_makneto->getConnection()->setStatus(XMPP::Status::DND);
}

void ConnectionView::invisibleClicked(bool)
{
	// check if we are yet logged in
	if(!m_makneto->getConnection()->isOnline())
		m_makneto->getConnection()->login();

	// set status to Invisible
	m_makneto->getConnection()->setStatus(XMPP::Status::Invisible);
}

void ConnectionView::offlineClicked(bool)
{
	if(m_makneto->getConnection()->isOnline())
		m_makneto->getConnection()->logout();
}

void ConnectionView::statusChanged(const XMPP::Status &status)
{
	setStatus(status);
}

void ConnectionView::setStatus(const XMPP::Status &status)
{
	switch (status.type())
	{
	case XMPP::Status::Online:
		m_buttonUserIcon->setIcon(KIcon("maknetoonline"));
		m_labelStatus->setText("<i>"+i18n("Online")+"</i>");
		break;
	case XMPP::Status::Away:
		m_buttonUserIcon->setIcon(KIcon("maknetoaway"));
		m_labelStatus->setText("<i>"+i18n("Away")+"</i>");
		break;
	case XMPP::Status::XA:
		m_buttonUserIcon->setIcon(KIcon("maknetoxa"));
		m_labelStatus->setText("<i>"+i18n("XA")+"</i>");
		break;
	case XMPP::Status::DND:
		m_buttonUserIcon->setIcon(KIcon("maknetodnd"));
		m_labelStatus->setText("<i>"+i18n("Do Not Disturb")+"</i>");
		break;
	case XMPP::Status::Invisible:
		m_buttonUserIcon->setIcon(KIcon("maknetoinvisible"));
		m_labelStatus->setText("<i>"+i18n("Invisible")+"</i>");
		break;
	case XMPP::Status::Offline:
		m_buttonUserIcon->setIcon(KIcon("maknetooffline"));
		m_labelStatus->setText("<i>"+i18n("Offline")+"</i>");
		break;
	default:
		break;
	}
}
