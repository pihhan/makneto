/*
 * sessiontabmanager.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "sessiontabmanager.h"
#include "sessionview.h"
#include "maknetohtmlbrowser.h"

#include "ktabbar.h"
#include "kiconloader.h"


#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QFrame>
#include <QDebug>

#include "xmpp_message.h"
#include "xmpp_jid.h"
#include "filetransfer.h"


#include <iostream>

SessionTabManager::SessionTabManager(Makneto *makneto, QWidget *): m_makneto(makneto)
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
  
  // Close tab on request
  connect(m_tab, SIGNAL(closeRequest(int)), this, SLOT(closeTab(int)));

	// we want to receive messages
	connect(makneto->getConnection(), SIGNAL(connMessageReceived(const Message &)), this, SLOT(messageReceived(const Message &)));
	connect(makneto->getConnection(), SIGNAL(connIncomingFileTransfer(FileTransfer *)), this, SLOT(incomingFileTransfer(FileTransfer *)));
  connect(makneto, SIGNAL(newSession(const QString &, ChatType)), this, SLOT(newSession(const QString &, ChatType)));

	// TODO: and status (from contact list?)

	// KHTMLpart widget for "Home tab"
	MaknetoHTMLBrowser *homeTabBrowser = new MaknetoHTMLBrowser(this);

	m_tab->addTab("Home");
	m_widgets->addWidget(homeTabBrowser);

	setLayout(m_mainlayout);
}

SessionTabManager::~SessionTabManager()
{

}

SessionView* SessionTabManager::newSessionTab(const QString &text)
{
	// add new tab
	m_tab->addTab(text);
	
	// create new session view and add to widgets
	SessionView *session = new SessionView(this, text, m_tab->count()-1);
	m_widgets->addWidget(session);

	connect(session, SIGNAL(sendMessage(const Message &)), m_makneto->getConnection(), SLOT(sendMessage(const Message &)));

	return session;
}

/**
*	@param 
*	@return session
*/

SessionView* SessionTabManager::findSession(const QString &jid)
{
	SessionView *session;

	// From 1!!! First widget is home tab! 
	for (int i=1; i<m_widgets->count(); i++)
	{
		session = dynamic_cast<SessionView*>(m_widgets->widget(i));

		if (session->jid() == jid)
			return session;
	}

	return 0;
}

void SessionTabManager::bringToFront(SessionView *session)
{
	std::cout << "SessionTabManager::bringToFront(" << session->id() << ")" << std::endl;
	
	m_tab->setCurrentIndex(session->id());	
}

void SessionTabManager::messageReceived(const Message &message)
{
	SessionView *session;

	// check for existing session
	session = findSession(message.from().bare());

	// no existing session - create new one
	if (!session)
		session = newSessionTab(message.from().bare());
  
  if (message.type().compare("groupchat") == 0)
    session->setType(1);
	
  // send proper message type to session
	if (!message.whiteboard().tagName().isEmpty())
		session->whiteboardMessage(message);
	else
		session->chatMessage(message);

	// bring to front
	bringToFront(session);
}

void SessionTabManager::incomingFileTransfer(FileTransfer *ft)
{
	std::cout << "SessionTabManager::incommingFileTransfer" << std::endl;

	SessionView *session;

	// check for existing session for peer
	session = findSession(ft->peer().bare());

	// no existing session - create new one
	if (!session)
		session = newSessionTab(ft->peer().bare());

	session->fileTransfer(ft);

	// bring to front
	bringToFront(session);
}

void SessionTabManager::closeTab(int tabIndex)
{
  std::cout << "SessionTabManager::closeTab" << std::endl;
  
  m_widgets->removeWidget(m_widgets->widget(tabIndex));
  m_tab->removeTab(tabIndex);
}

void SessionTabManager::newSession(const QString &text, ChatType type)
{
	std::cout << "SessionTabManager::newSession << std::endl";

	SessionView *session;

	// create new session
	session = newSessionTab(text);
  session->setType(type);

	// bring to front
	bringToFront(session);
}
