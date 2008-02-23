/*
 * maknetoview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#include "maknetoview.h"
#include "sidebarwidget.h"
#include "connectionview.h"
#include "roasterview.h"
#include "kiconloader.h"
#include "klocale.h"

#include <klocale.h>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <kmultitabbar.h>

MaknetoView::MaknetoView(QWidget *)
{

	QHBoxLayout *layout = new QHBoxLayout;

	SidebarWidget *sidebar = new SidebarWidget(this);

	ConnectionView *m_conn = new ConnectionView(this);
	RoasterView *m_roaster = new RoasterView(this);

	sidebar->appendTabWidget(m_conn, KIconLoader::global()->loadIcon("connect-no", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 0, i18n("Session"));

	sidebar->appendTabWidget(m_roaster, KIconLoader::global()->loadIcon("users", KIconLoader::Toolbar, KIconLoader:: SizeSmall), 1, i18n("Roaster"));

	sidebar->setCurrentIndex(0);

	layout->addWidget(sidebar);
	setLayout(layout);
}

MaknetoView::~MaknetoView()
{

}

void MaknetoView::switchColors()
{
}

#include "maknetoview.moc"
