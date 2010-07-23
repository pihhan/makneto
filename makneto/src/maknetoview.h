/*
 * maknetoview.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */
#ifndef MAKNETOVIEW_H
#define MAKNETOVIEW_H

#include <QtGui/QWidget>
#include "makneto.h"

class QPainter;
class KUrl;
class SessionView;
class SessionTabManager;

/**
 * This is the main view class for Makneto.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class Makneto;
class SidebarWidget;
class MUCView;
class LocalPreview;

class MaknetoView : public QWidget
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    MaknetoView(QWidget *parent, Makneto *makneto);

    /**
     * Destructor
     */
    virtual ~MaknetoView();
    
    SessionTabManager *getSessionTabManager(void) { return m_sessiontabmanager; }
    SidebarWidget *getSidebarWidget(void) { return m_sidebar; }
    MUCView *getMUCView() { return m_muc; }

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);


private slots:
	void settingsChanged();

private:
  SidebarWidget *m_sidebar;
  SessionView *m_sessionview;
  SessionTabManager *m_sessiontabmanager;
  LocalPreview *m_preview;
  MUCView *m_muc;
};

#endif // MaknetoVIEW_H
