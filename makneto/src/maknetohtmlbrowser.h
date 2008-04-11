/*
 * maknetohtmlbrowser.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETOHTMLBROWSER_H
#define MAKNETOHTMLBROWSER_H

#include <QtGui/QWidget>

class QVBoxLayout;

/**
 * This is the a Makneto HTML Browser for "Home tab".
 *
 * @short Makneto HTML Browser
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 */

class MaknetoHTMLBrowser : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	MaknetoHTMLBrowser(QWidget *parent);
	
	/**
	* Destructor
	*/
	virtual ~MaknetoHTMLBrowser() { }
	
private:
	QVBoxLayout *mainLayout;
};

#endif //MAKNETOHTMLBROWSER_H
