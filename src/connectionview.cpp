/*
 * connectionview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "connectionview.h"
#include "kiconloader.h"
#include "klocale.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>


ConnectionView::ConnectionView(QWidget *)
{
	m_buttonslayout = new QVBoxLayout(this);
	m_buttonslayout->setMargin(0);
	m_buttonslayout->setSpacing(0);

	m_buttononline = new QPushButton(KIconLoader::global()->loadIcon("hi16-action-jabber_online", KIconLoader::Toolbar, KIconLoader:: SizeSmall), "&Online", this);
	m_buttonslayout->addWidget(m_buttononline);

	m_buttonaway = new QPushButton("&Away", this);
	m_buttonslayout->addWidget(m_buttonaway);

	m_buttondnd = new QPushButton("&Do not disturb", this);
	m_buttonslayout->addWidget(m_buttondnd);

	m_buttonoffline = new QPushButton("O&ffline", this);
	m_buttonslayout->addWidget(m_buttonoffline);

	setLayout(m_buttonslayout);
}

ConnectionView::~ConnectionView()
{

}

