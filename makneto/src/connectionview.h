/*
 * connectionview.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef CONNECTIONVIEW_H
#define CONNECTIONVIEW_H

#include <QtGui/QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QGridLayout;
class QLabel;
class Connection;
class Makneto;

namespace XMPP
{
	class Status;
}

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

public slots:
	void statusChanged(const XMPP::Status &);

private:
	QVBoxLayout *m_mainlayout;
	QVBoxLayout *m_buttonslayout;
	QHBoxLayout *m_stateLayout;

	QPushButton *m_buttonUserIcon;
	QPushButton *m_buttononline;
	QPushButton *m_buttonaway;
	QPushButton *m_buttonxa;
	QPushButton *m_buttondnd;
	QPushButton *m_buttoninvisible;
	QPushButton *m_buttonoffline;

	QLabel *m_labelJabberID;
	QLabel *m_labelStatus;

	Connection *m_conn;
	Makneto *m_makneto;

	void setStatus(const XMPP::Status &status);
};

#endif // CONNECTIONVIEW_H
