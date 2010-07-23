/*
 * maknetomainwindow.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#ifndef MAKNETOMAINWINDOW_H
#define MAKNETOMAINWINDOW_H

#include <kxmlguiwindow.h>
#include <kapplication.h>
#include "ui_prefs_base.h"
#include "ui_prefs_media.h"
#include "ui_prefs_medianet.h"
#include "makneto.h"

class MaknetoView;
class KToggleAction;
class KAction;
class KUrl;

/**
 * This class serves as the main window for Makneto.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class Makneto;

class MaknetoMainWindow : public KXmlGuiWindow
{
	Q_OBJECT
public:
	MaknetoMainWindow(Makneto *makneto);
  
  MaknetoView *getMaknetoView(void) { return m_view; }
	
	virtual ~MaknetoMainWindow();

protected:
        virtual bool queryClose();

private slots:
  void hideAll();
	void fullScreen();
	void showMenubar();
	void quit();
	void optionsPreferences();
        void showSoundPlayer();

private:
	void setupActions();
	
	Ui::Account ui_prefs_account ;
        Ui::MediaConfig   ui_prefs_media;
        Ui::MediaNet    ui_prefs_medianet;
	MaknetoView *m_view;
	Makneto *m_makneto;
	
	KToggleAction *m_toolbarAction;
	KToggleAction *m_statusbarAction;
  
  KAction *m_fullScreenAction, *hideAllAction;
  KAction *m_saveFeatures;
  KAction *m_soundFilePlayer;
  
};

#endif // _MAKNETOMAINWINDOW_H_
