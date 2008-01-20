/*
 * maknetoview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#include "maknetoview.h"
#include "sidebarwidget.h"

#include <klocale.h>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <kmultitabbar.h>

MaknetoView::MaknetoView(QWidget *)
{

	QHBoxLayout *layout = new QHBoxLayout;

	SidebarWidget *sidebar = new SidebarWidget(this);

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
