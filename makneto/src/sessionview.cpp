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

#include "xmpp_status.h"

SessionView::SessionView(QWidget *)
{
	m_mainlayout = new QVBoxLayout(this);
	m_mainlayout->setMargin(0);
	m_mainlayout->setSpacing(0);

	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Vertical);

	// add whiteboard widget
	m_wbwidget = new WbWidget("s1", "rezza@jabber.cz", QSize(300, 300), m_splitter);
	m_mainlayout->addWidget(m_wbwidget);

	m_chatlayout = new QVBoxLayout(m_splitter);

	m_chatoutput = new QTextEdit(this);
	m_chatinput = new QTextEdit(this);

	m_chatlayout->addWidget(m_chatoutput);
	m_chatlayout->addWidget(m_chatinput);

	// splitter
	m_splitter->addWidget(m_wbwidget);
	//m_splitter->addWidget(m_chatlayout);

	m_mainlayout->addWidget(m_splitter);
	

	setLayout(m_mainlayout);
}

SessionView::~SessionView()
{

}
