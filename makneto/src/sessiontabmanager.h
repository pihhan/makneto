/*
 * sessiontabmanager.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef SESSIONTABMANAGER_H
#define SESSIONTABMANAGER_H

#include <QtGui/QWidget>

class KMultiTabBar;
class QStackedWidget;
class QVBoxLayout;
class QTabBar;

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
	SessionTabManager(QWidget *parent);

	/**
	* Destructor
	*/
	virtual ~SessionTabManager();

	void newSessionTab(const QString &text);
private:
	QVBoxLayout *m_mainlayout;

	QTabBar *m_multitab;
	QStackedWidget *m_widgets;
};

#endif // SESSIONTABMANAGER_H
