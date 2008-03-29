/*
 * sessiontabmanager.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "sessiontabmanager.h"
#include "sessionview.h"

#include "kmultitabbar.h"
#include "kiconloader.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabBar>

#include "xmpp_status.h"

SessionTabManager::SessionTabManager(QWidget *)
{
	m_mainlayout = new QVBoxLayout(this);
	m_mainlayout->setMargin(0);
	m_mainlayout->setSpacing(0);

	//m_multitab = new KMultiTabBar(KMultiTabBar::Top, this);
	m_multitab = new QTabBar(this);
	m_widgets = new QStackedWidget();

	SessionView *s = new SessionView(this);

	m_mainlayout->addWidget(m_multitab);
	//m_mainlayout->addWidget(m_widgets);
	m_mainlayout->addWidget(s);


	//m_multitab->setPosition(KMultiTabBar::Top);

	// TEST
	newSessionTab("rezza@jabber.cz");

	setLayout(m_mainlayout);
}

SessionTabManager::~SessionTabManager()
{

}

void SessionTabManager::newSessionTab(const QString &text)
{
	//m_multitab->appendTab(KIconLoader::global()->loadIcon("konqueror", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 0, text);

	m_multitab->addTab(text);
}
