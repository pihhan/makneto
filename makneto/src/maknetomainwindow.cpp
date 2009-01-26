/*
 * makneto.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#include "maknetomainwindow.h"
#include "maknetoview.h"
#include "settings.h"
#include "mediaplayer.h"

#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>

#include <QtCore/QBuffer>

#include <kconfigdialog.h>
#include <kstatusbar.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>

#include <KDE/KLocale>
#include <KDE/KMenuBar>

#include <Phonon/MediaSource>
#include "ftstream.h"

MaknetoMainWindow::MaknetoMainWindow(Makneto *makneto) : KXmlGuiWindow(), m_view(new MaknetoView(this, makneto))
{
	m_makneto = makneto;
	
	setAcceptDrops(true);
	
	setCentralWidget(m_view);
	
	// then, setup our actions
	setupActions();
	
	// add a status bar
	statusBar()->show();
	
	setupGUI();
}

MaknetoMainWindow::~MaknetoMainWindow()
{
}

void MaknetoMainWindow::setupActions()
{
	KStandardAction::fullScreen(this, SLOT(fullScreen()), this, actionCollection());
	KStandardAction::showMenubar(this, SLOT(showMenubar()), actionCollection());
	KStandardAction::quit(this, SLOT(quit()), actionCollection());
	
	KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
}

void
 MaknetoMainWindow::quit()
{
  if(m_makneto->getConnection()->isOnline())
    m_makneto->getConnection()->logout();
  close();
	//m_makneto->getConnection()->clientDisconnect();
}

void MaknetoMainWindow::optionsPreferences()
{
	if ( KConfigDialog::showDialog( "settings" ) )  {
		return;
	}

	KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());

	QWidget *accountSettingsDlg = new QWidget;
	ui_prefs_account.setupUi(accountSettingsDlg);
	dialog->addPage(accountSettingsDlg, i18n("Account"), "preferences-system-network");

// 	QWidget *fileTransferSettingsDlg = new QWidget;
// 	ui_prefs_file_transfer.setupUi(fileTransferSettingsDlg);
// 	dialog->addPage(fileTransferSettingsDlg, i18n("File transfer"), "preferences-other");

	connect(dialog, SIGNAL(settingsChanged(QString)), m_view, SLOT(settingsChanged()));
	dialog->setAttribute( Qt::WA_DeleteOnClose );
	dialog->show();
}

void MaknetoMainWindow::fullScreen()
{
	// set fullscreen state
	setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void MaknetoMainWindow::showMenubar()
{
	menuBar()->setVisible(!menuBar()->isVisible());
}

#include "maknetomainwindow.moc"
