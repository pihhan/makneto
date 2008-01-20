/*
 * connectionview.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef CONNECTIONVIEW_H
#define CONNECTIONVIEW_H

#include <QtGui/QWidget>
#include "xmpp.h"

class QVBoxLayout;
class QPushButton;
class QGridLayout;

/**
 * This is connectio view widget for Makneto
 *
 * @short Connection view widget
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class ConnectionView : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	ConnectionView(QWidget *parent);

	/**
	* Destructor
	*/
	virtual ~ConnectionView();

private:
	QVBoxLayout *m_mainlayout;
	QVBoxLayout *m_buttonslayout;
	QGridLayout *m_infolayout;

	QPushButton *m_buttononline;
	QPushButton *m_buttonaway;
	QPushButton *m_buttondnd;
	QPushButton *m_buttonoffline;
};

#endif // CONNECTIONVIEW_H
