/*
 * sidebarwidget.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "sidebarwidget.h"
#include "connectionview.h"
#include "kmultitabbar.h"
#include "kiconloader.h"
#include "klocale.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>


SidebarWidget::SidebarWidget(QWidget *)
{
	m_layout = new QHBoxLayout(this);
	m_layout->setMargin(0);
	m_layout->setSpacing(0);

	m_multitab = new KMultiTabBar(KMultiTabBar::Left, this);
	m_conn = new ConnectionView(this);

	m_layout->addWidget(m_multitab);
	m_layout->addWidget(m_conn);

	m_multitab->setPosition(KMultiTabBar::Left);

	m_multitab->appendTab(KIconLoader::global()->loadIcon("connect-no", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 1, i18n("Session"));

	

	m_multitab->appendTab(KIconLoader::global()->loadIcon("users", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 2, i18n("Contacts"));
	m_multitab->appendTab(KIconLoader::global()->loadIcon("favorites", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 3, i18n("Maknets"));

	m_multitab->setTab(1, true);

	setLayout(m_layout);
}

SidebarWidget::~SidebarWidget()
{

}

