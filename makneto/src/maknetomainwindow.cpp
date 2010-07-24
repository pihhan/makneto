/*
 * makneto.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#include "maknetomainwindow.h"
#include "maknetoview.h"
#include "settings.h"
#include "mediaplayer.h"
#include "sessiontabmanager.h"
#include "sessionview.h"
#include "sidebarwidget.h"

#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>

#include <QtCore/QBuffer>

#include <kconfigdialog.h>
#include <kstatusbar.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <ktogglefullscreenaction.h>

#include <KDE/KLocale>
#include <KDE/KMenuBar>

#include <Phonon/MediaSource>
#include "ftstream.h"
#include "featurelist.h"
#include "soundfiletester.h"

MaknetoMainWindow::MaknetoMainWindow(Makneto *makneto) : KXmlGuiWindow()
{
        m_view = new MaknetoView(this, makneto);
	m_makneto = makneto;
	
	setAcceptDrops(true);
	
	setCentralWidget(m_view);
	
	// then, setup our actions
	setupActions();
	
	// add a status bar
	statusBar()->show();
	
	setupGUI();
  
  QList<QAction *> a = menuBar()->actions();
  if (a.size() > 1 && a[1]->menu()) {
    a[1]->menu()->insertAction(m_fullScreenAction, hideAllAction);
    a[1]->menu()->insertAction(hideAllAction, m_saveFeatures);
  }
}

MaknetoMainWindow::~MaknetoMainWindow()
{
}

void MaknetoMainWindow::setupActions()
{
  hideAllAction = actionCollection()->addAction("hideAll");
  hideAllAction->setText(tr("Hide All"));
  hideAllAction->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_F);
  hideAllAction->setMenuRole(QAction::PreferencesRole);
  connect(hideAllAction, SIGNAL(triggered()), this, SLOT(hideAll()));
  m_fullScreenAction = KStandardAction::fullScreen(this, SLOT(fullScreen()), this, actionCollection());

  m_saveFeatures = actionCollection()->addAction("writeFeatures");
  m_saveFeatures->setText(tr("Save &features"));
  m_saveFeatures->setMenuRole(QAction::PreferencesRole);
  connect(m_saveFeatures, SIGNAL(triggered()), m_makneto->getFeatureManager(), SLOT(writeDatabase()));

  m_soundFilePlayer = actionCollection()->addAction(
        "soundFilePlayer", this, SLOT(showSoundPlayer()) );
  m_soundFilePlayer->setText(tr("&Play sound file"));
//  m_soundFilePlayer->setMenuRole(QAction::ApplicationSpecificRole);
  m_soundFilePlayer->setMenuRole(QAction::PreferencesRole);

    KStandardAction::showMenubar(this, SLOT(showMenubar()), actionCollection());
    KStandardAction::quit(this, SLOT(quit()), actionCollection());
    
    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
}

void MaknetoMainWindow::quit()
{
  close();
  FeatureListManager *flm = m_makneto->getFeatureManager();
  if (flm) {
      if (flm->modified())
          flm->writeDatabase();
  }
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

        QTabWidget *mediaSettingsDlg = new QTabWidget();
        ui_prefs_media.setupUi(mediaSettingsDlg);
        dialog->addPage(mediaSettingsDlg, i18n("Media"), "audiocall");

        QFrame *mediaNetSettingsDlg = new QFrame();
        ui_prefs_medianet.setupUi(mediaNetSettingsDlg);
        dialog->addPage(mediaNetSettingsDlg, i18n("Network"), "webcamsend");

// 	QWidget *fileTransferSettingsDlg = new QWidget;
// 	ui_prefs_file_transfer.setupUi(fileTransferSettingsDlg);
// 	dialog->addPage(fileTransferSettingsDlg, i18n("File transfer"), "preferences-other");

	connect(dialog, SIGNAL(settingsChanged(QString)), m_view, SLOT(settingsChanged()));
	dialog->setAttribute( Qt::WA_DeleteOnClose );
	dialog->show();
}

void MaknetoMainWindow::hideAll()
{
  SessionView *session = m_view->getSessionTabManager()->activeSession();
  if (session)
  {
    session->showHideChat();
  }
  m_view->getSidebarWidget()->hideAll();
  showMenubar();
  statusBar()->setVisible(!statusBar()->isVisible());
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


bool MaknetoMainWindow::queryClose()
{
  m_makneto->getConnection()->logout();
  return true;
}

void MaknetoMainWindow::showSoundPlayer()
{
    SoundFileTester * sfp = new SoundFileTester(
        m_makneto->getMediaManager(), NULL);
    sfp->setVisible(true);
}

#include "maknetomainwindow.moc"

