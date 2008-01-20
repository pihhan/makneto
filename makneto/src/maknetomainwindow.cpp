/*
 * makneto.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#include "maknetomainwindow.h"
#include "maknetoview.h"

#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>

#include <kconfigdialog.h>
#include <kstatusbar.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>

#include <KDE/KLocale>

MaknetoMainWindow::MaknetoMainWindow()
    : KXmlGuiWindow(),
    m_view(new MaknetoView(this))
{
    setAcceptDrops(true);

    setCentralWidget(m_view);

    // then, setup our actions
    setupActions();

    // add a status bar
    statusBar()->show();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI();
}

MaknetoMainWindow::~MaknetoMainWindow()
{
}

void MaknetoMainWindow::setupActions()
{
    KStandardAction::openNew(this, SLOT(fileNew()), actionCollection());
    KStandardAction::quit(this, SLOT(quit()), actionCollection());

    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
}

void MaknetoMainWindow::fileNew()
{
}

void MaknetoMainWindow::optionsPreferences()
{
}

#include "maknetomainwindow.moc"
