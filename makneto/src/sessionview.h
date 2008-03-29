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

#include "xmpp_chatstate.h"

namespace XMPP 
{
	class Message;
}

using namespace XMPP;

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
	SessionView(QWidget *parent, const QString &jid);

	/**
	* Destructor
	*/
	virtual ~SessionView();

	QString session() { return m_session; }
	QString jid() { return m_jid; }

	void chatMessage(const Message &message); 

public slots:
	void sendClicked();

signals:
	void sendMessage(const Message &);

private:
	QVBoxLayout *m_mainlayout;
	QVBoxLayout *m_chatlayout;
	QHBoxLayout *m_bottomlayout;

	QSplitter *m_splitter;

	WbWidget *m_wbwidget;
	
	QTextEdit *m_chatoutput;
	QTextEdit *m_chatinput;

	QPushButton *m_sendmsg;
	
	QString m_session;
	QString m_jid;

	ChatState m_lastChatState;
};

#endif // SESSIONVIEW_H
