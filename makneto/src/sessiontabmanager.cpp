/*
 * sessiontabmanager.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "sessiontabmanager.h"
#include "sessionview.h"

#include "ktabbar.h"
#include "kiconloader.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QStackedWidget>

#include "xmpp_message.h"
#include "xmpp_jid.h"

SessionTabManager::SessionTabManager(Makneto *makneto, QWidget *)
{
	m_mainlayout = new QVBoxLayout(this);
	m_mainlayout->setMargin(5);
	m_mainlayout->setSpacing(5);

	m_tab = new KTabBar(this);
	m_tab->setHoverCloseButton(true);

	m_widgets = new QStackedWidget();

	m_mainlayout->addWidget(m_tab);
	m_mainlayout->addWidget(m_widgets);

	// connect tab bar with widgets
	connect(m_tab, SIGNAL(currentChanged(int)), m_widgets, SLOT(setCurrentIndex(int)));

	// we want to receive messages
	connect(makneto->getConnection(), SIGNAL(connMessageReceived(const Message &)), this, SLOT(messageReceived(const Message &)));

	setLayout(m_mainlayout);
}

SessionTabManager::~SessionTabManager()
{

}

void SessionTabManager::newSessionTab(const QString &text)
{
	// add new tab
	m_tab->addTab(text);
	
	// create new session view and add to widgets
	SessionView *session = new SessionView(this, text);
	m_widgets->addWidget(session);
}

SessionView* SessionTabManager::findSession(const QString &jid)
{
	SessionView *session;

	for (int i=0; i<m_widgets->count(); i++)
	{
		session = dynamic_cast<SessionView*>(m_widgets->widget(i));

		if (session->jid() == jid)
			return session;
	}

	return 0;
}

void SessionTabManager::messageReceived(const Message &message)
{
	SessionView *session;

	session = findSession(message.from().full());

	if (!session)
		newSessionTab(message.from().full());
	else
	{
		session->chatMessage(message);
	}
}
