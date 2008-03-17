/*
 * sessionview.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QtGui/QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QTextEdit;
class WbWidget;
class QSplitter;

/**
 * This is session view widget for Makneto
 *
 * @short Session view widget
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class SessionView : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	SessionView(QWidget *parent);

	/**
	* Destructor
	*/
	virtual ~SessionView();

private:
	QVBoxLayout *m_mainlayout;
	QVBoxLayout *m_chatlayout;
	QHBoxLayout *m_bottomlayout;

	QSplitter *m_splitter;

	WbWidget *m_wbwidget;
	
	QTextEdit *m_chatoutput;
	QTextEdit *m_chatinput;

	QPushButton *m_sendmsg;
};

#endif // SESSIONVIEW_H
