/*
 * makneto.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETO_H
#define MAKNETO_H

#include "maknetomainwindow.h"
#include "connection.h"

#include <QObject>

/**
 * This is Makneto class - central class of Makneto suite
 *
 * @short Makneto
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class MaknetoMainWindow;
class RoasterView;

class Makneto : public QObject
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	Makneto(QObject *parent=0);

	/**
	* Destructor
	*/
	virtual ~Makneto();

	void setMainWindow(MaknetoMainWindow *mainWindow);
	void setRosterView(RoasterView *roster);
	Connection *getConnection() { return m_conn; }

private:
	MaknetoMainWindow *m_mainWindow;
	Connection *m_conn;
	RoasterView *m_roster;
};

#endif // MAKNETO_H
