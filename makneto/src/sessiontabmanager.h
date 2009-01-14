/*
 * sessiontabmanager.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef SESSIONTABMANAGER_H
#define SESSIONTABMANAGER_H

#include "makneto.h"

#include <QtGui/QWidget>

class QStackedWidget;
class QVBoxLayout;
class KTabBar;
class SessionView;

/**
 * This is session tab manager widget for Makneto
 *
 * @short Session tab manager widget
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class SessionTabManager : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	SessionTabManager(Makneto *makneto, QWidget *parent);

	/**
	* Destructor
	*/
	virtual ~SessionTabManager();

	SessionView *newSessionTab(const QString &text);
	SessionView *findSession(const QString &jid);
	void bringToFront(SessionView *session);
public slots:
	void messageReceived(const Message &message);
	void newSession(const QString &text);
	void incomingFileTransfer(FileTransfer *ft);
  void closeTab(int tabIndex);

private:
	QVBoxLayout *m_mainlayout;

	KTabBar *m_tab;
	QStackedWidget *m_widgets;
	Makneto *m_makneto;
};

#endif // SESSIONTABMANAGER_H
