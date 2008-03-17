/*
 * makneto.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#include "maknetomainwindow.h"
#include "maknetoview.h"
#include "settings.h"

#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>

#include <kconfigdialog.h>
#include <kstatusbar.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>

#include <KDE/KLocale>

MaknetoMainWindow::MaknetoMainWindow(Makneto *makneto)
    : KXmlGuiWindow(),
    m_view(new MaknetoView(this, makneto))
{
	m_makneto = makneto;

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
//    KStandardAction::quit(this, SLOT(quit()), actionCollection());

    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
}

void MaknetoMainWindow::fileNew()
{
}

void MaknetoMainWindow::optionsPreferences()
{
	if ( KConfigDialog::showDialog( "settings" ) )  {
		return;
	}

	KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
	QWidget *generalSettingsDlg = new QWidget;
	ui_prefs_base.setupUi(generalSettingsDlg);
	dialog->addPage(generalSettingsDlg, i18n("General"), "package_setting");
	connect(dialog, SIGNAL(settingsChanged(QString)), m_view, SLOT(settingsChanged()));
	dialog->setAttribute( Qt::WA_DeleteOnClose );
	dialog->show();
}

#include "maknetomainwindow.moc"
