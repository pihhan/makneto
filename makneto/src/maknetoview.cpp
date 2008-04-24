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
#include "sessiontabmanager.h"
#include "kiconloader.h"
#include "klocale.h"
#include "makneto.h"
#include "maknetocontactlist.h"

#include <klocale.h>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <kmultitabbar.h>
#include <QtGui/QSplitter>

MaknetoView::MaknetoView(QWidget *, Makneto *makneto)
{

	QHBoxLayout *layout = new QHBoxLayout;

	// prepare sidebar
	SidebarWidget *sidebar = new SidebarWidget(this);

	ConnectionView *m_conn = new ConnectionView(this, makneto);
	RoasterView *m_roaster = new RoasterView(this, makneto);

	sidebar->appendTabWidget(m_conn, KIconLoader::global()->loadIcon("konqueror", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 0, i18n("Network"));

	sidebar->appendTabWidget(m_roaster, KIconLoader::global()->loadIcon("system-users", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 1, i18n("Contacts"));

	sidebar->setCurrentIndex(0);
	sidebar->setMaximumWidth(250); 

	// session manager
	m_sessiontabmanager = new SessionTabManager(makneto, this);

	// add to layout
	layout->addWidget(sidebar);
	layout->addWidget(m_sessiontabmanager);

	setLayout(layout);
}

MaknetoView::~MaknetoView()
{

}

void MaknetoView::settingsChanged()
{

}

#include "maknetoview.moc"
