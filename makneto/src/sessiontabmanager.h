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

	void newSessionTab(const QString &text);
	SessionView *findSession(const QString &jid);
public slots:
	void messageReceived(const Message &message);
 
private:
	QVBoxLayout *m_mainlayout;

	KTabBar *m_tab;
	QStackedWidget *m_widgets;
};

#endif // SESSIONTABMANAGER_H
