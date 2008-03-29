/*
 * sessionview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "sessionview.h"
#include "kiconloader.h"
#include "klocale.h"
#include "wbwidget.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>
#include <QtGui/QSplitter>

#include "xmpp_message.h"
#include "xmpp_chatstate.h"
#include "xmpp_jid.h"

SessionView::SessionView(QWidget *, const QString &jid): m_jid(jid), m_lastChatState(StateNone)
{
	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Vertical);

	m_mainlayout = new QVBoxLayout(this);
	m_mainlayout->setMargin(0);
	m_mainlayout->setSpacing(0);

	// add whiteboard widget
	m_wbwidget = new WbWidget("s1", "rezza@jabber.cz", QSize(300, 300), m_splitter);
	m_mainlayout->addWidget(m_wbwidget);

	m_chatlayout = new QVBoxLayout();

	m_chatoutput = new QTextEdit(this);
	m_chatinput = new QTextEdit(this);
	m_sendmsg = new QPushButton("&Send", this);
	connect(m_sendmsg, SIGNAL(clicked()), this, SLOT(sendClicked()));

	m_chatoutput->setTextFormat(Qt::RichText);
	m_chatoutput->setReadOnly(true);

	m_chatlayout->addWidget(m_chatoutput);
	m_chatlayout->addWidget(m_chatinput);
	m_chatlayout->addWidget(m_sendmsg);
	
	m_mainlayout->addWidget(m_wbwidget);
	m_mainlayout->addLayout(m_chatlayout);

	setLayout(m_mainlayout);
}

SessionView::~SessionView()
{

}

void SessionView::sendClicked()
{
	QString text;
	Message message;

	// prepare message
	message.setTo(m_jid);
	message.setType("chat");
	message.setChatState(StateActive);
	message.setBody(m_chatinput->text());

	emit sendMessage(message);

	// format for chat window
	text = "<b>Me</b>: "+m_chatinput->text();

	m_chatinput->setText("");

	// show text in chat window
	m_chatoutput->append(text);
}

void SessionView::chatMessage(const Message &message)
{
	QString text;

	switch (message.chatState())
	{
		case StateComposing:
			if (m_lastChatState != StateComposing)
				text += "<i>"+message.from().bare()+" is composing message</i>";
			else
				return;
			break;

		case StateGone:
			text += "<i>"+message.from().bare()+" has gone</i>";
			break;

		case StateInactive:
		case StatePaused:
		case StateNone:
		case StateActive:
		default:
			if (!message.body().isEmpty())
				text = "<b>"+message.from().bare()+"</b>: "+message.body();
			else
			{
				m_lastChatState = message.chatState();
				return;
			}
	}

	m_chatoutput->append(text);

	m_lastChatState = message.chatState();
}
