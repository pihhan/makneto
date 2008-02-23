/*
 * roasterview.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef ROASTERVIEW_H
#define ROASTERVIEW_H

#include <QtGui/QWidget>
#include "xmpp.h"

class QVBoxLayout;
class QPushButton;
class QGridLayout;
class KListWidgetSearchLine;
class QTreeView;

/**
 * This is roaster view widget for Makneto
 *
 * @short Roaster view widget
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class RoasterView : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	RoasterView(QWidget *parent);

	/**
	* Destructor
	*/
	virtual ~RoasterView();

private:
	QVBoxLayout *m_mainlayout;

	QTreeView *m_roster;
};

#endif // ROASTERVIEW_H
