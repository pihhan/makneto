/*
 * maknetoview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#include "maknetoview.h"
#include "sidebarwidget.h"
#include "connectionview.h"
#include "roasterview.h"
#include "sessionview.h"
#include "kiconloader.h"
#include "klocale.h"
#include "makneto.h"

#include <klocale.h>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <kmultitabbar.h>

MaknetoView::MaknetoView(QWidget *, Makneto *makneto)
{

	QHBoxLayout *layout = new QHBoxLayout;

	// prepare sidebar
	SidebarWidget *sidebar = new SidebarWidget(this);

	ConnectionView *m_conn = new ConnectionView(this);
	RoasterView *m_roaster = new RoasterView(this);

	makneto->setRosterView(m_roaster);

	sidebar->appendTabWidget(m_conn, KIconLoader::global()->loadIcon("connect-no", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 0, i18n("Session"));

	sidebar->appendTabWidget(m_roaster, KIconLoader::global()->loadIcon("users", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 1, i18n("Roaster"));

	sidebar->setCurrentIndex(0);
	sidebar->setMaximumWidth(250); 

	// prepare whiteboard view
	m_sessionview = new SessionView(this);

	// add to layout
	layout->addWidget(sidebar);
	layout->addWidget(m_sessionview);
	setLayout(layout);
}

MaknetoView::~MaknetoView()
{

}

void MaknetoView::switchColors()
{
}

#include "maknetoview.moc"
