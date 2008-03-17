/*
 * makneto.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "makneto.h"
#include "maknetomainwindow.h"

#include <QObject>

Makneto::Makneto(QObject *parent) : QObject(parent)
{
	m_mainWindow = 0;
	m_roster = 0;

	// set up connection
	m_conn = new Connection();
}

Makneto::~Makneto()
{

}

void Makneto::setMainWindow(MaknetoMainWindow *mainWindow)
{
	m_mainWindow = mainWindow;
}

void Makneto::setRosterView(RoasterView *roster)
{
	m_roster = roster;
}

#include "makneto.moc"

