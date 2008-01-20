/*
 * sidebarwidget.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QtGui/QWidget>

class KMultiTabBar;
class QHBoxLayout;
class ConnectionView;

/**
 * This is sidebar widget for Makneto
 *
 * @short Sidebar widget
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class SidebarWidget : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	SidebarWidget(QWidget *parent);

	/**
	* Destructor
	*/
	virtual ~SidebarWidget();

private:
	QHBoxLayout *m_layout;
	ConnectionView *m_conn;
	KMultiTabBar *m_multitab;
};

#endif // SIDEBARWIDGET_H
