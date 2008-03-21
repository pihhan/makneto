/*
 * connectionview.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef CONNECTIONVIEW_H
#define CONNECTIONVIEW_H

#include <QtGui/QWidget>

class QVBoxLayout;
class QPushButton;
class QGridLayout;
class Connection;
class Makneto;

/**
 * This is connection view widget for Makneto
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
	ConnectionView(QWidget *parent, Makneto *makneto);

	/**
	* Destructor
	*/
	virtual ~ConnectionView();

private slots:
	void onlineClicked(bool);
	void awayClicked(bool);
	void xaClicked(bool);
	void dndClicked(bool);
	void invisibleClicked(bool);
	void offlineClicked(bool);

private:
	QVBoxLayout *m_mainlayout;
	QVBoxLayout *m_buttonslayout;
	QGridLayout *m_infolayout;

	QPushButton *m_buttononline;
	QPushButton *m_buttonaway;
	QPushButton *m_buttonxa;
	QPushButton *m_buttondnd;
	QPushButton *m_buttoninvisible;
	QPushButton *m_buttonoffline;

	Connection *m_conn;
	Makneto *m_makneto;
};

#endif // CONNECTIONVIEW_H
