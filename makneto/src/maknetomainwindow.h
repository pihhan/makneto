/*
 * maknetomainwindow.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#ifndef MAKNETOMAINWINDOW_H
#define MAKNETOMAINWINDOW_H

#include <kxmlguiwindow.h>
#include "ui_prefs_base.h"
#include "makneto.h"

class MaknetoView;
class KToggleAction;
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

    virtual ~MaknetoMainWindow();

private slots:
    void fileNew();
    void optionsPreferences();

private:
    void setupActions();

private:
    Ui::Account ui_prefs_base ;
    MaknetoView *m_view;
    Makneto *m_makneto;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // _MAKNETOMAINWINDOW_H_
