/*
 * roasterview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "roasterview.h"
#include "klistwidgetsearchline.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QTreeView>

RoasterView::RoasterView(QWidget *)
{
	m_mainlayout = new QVBoxLayout(this);
	m_mainlayout->setMargin(0);
	m_mainlayout->setSpacing(0);

	// TODO: toolbar
	// buttons: add new contact to roaster etc...

	// roster
	m_roster = new QTreeView(this);

	// TODO: search button for roaster

	// TODO: filter button
	// filter for online/offline contacts in roaster
	
	m_mainlayout->addWidget(m_roster);

	setLayout(m_mainlayout);
}

RoasterView::~RoasterView()
{

}

