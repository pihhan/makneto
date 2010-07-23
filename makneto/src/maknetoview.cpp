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
#include "mucview.h"
#include "localpreview.h"

#include <klocale.h>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <kmultitabbar.h>
#include <QtGui/QSplitter>

MaknetoView::MaknetoView(QWidget *, Makneto *makneto)
{

	QHBoxLayout *layout = new QHBoxLayout;

	// prepare sidebar
	m_sidebar = new SidebarWidget(this);

	ConnectionView *m_conn = new ConnectionView(this, makneto);
	RoasterView *m_roaster = new RoasterView(this, makneto);
  m_muc = new MUCView(this, makneto);

	m_sidebar->appendTabWidget(m_conn, KIconLoader::global()->loadIcon("konqueror", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 0, i18n("Network"));

	m_sidebar->appendTabWidget(m_roaster, KIconLoader::global()->loadIcon("system-users", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 1, i18n("Contacts"));
  
  m_sidebar->appendTabWidget(m_muc, KIconLoader::global()->loadIcon("goto", KIconLoader::Toolbar, KIconLoader::SizeSmall), 2, i18n("MUC"));

  LocalPreview *m_preview = new LocalPreview(this, makneto);
  m_sidebar->appendTabWidget(m_preview, 
    KIconLoader::global()->loadIcon("media-record", KIconLoader::Toolbar, KIconLoader::SizeSmall), 3, i18n("Preview"));

	m_sidebar->setCurrentIndex(0);
	m_sidebar->setMaximumWidth(250); 

	// session manager
	m_sessiontabmanager = new SessionTabManager(makneto, this);

	// add to layout
	layout->addWidget(m_sidebar);
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
