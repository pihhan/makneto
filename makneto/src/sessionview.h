/*
 * sessionview.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QtGui/QWidget>
#include <QByteArray>

#include "mediaplayer.h"

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QTextEdit;
class WbWidget;
class QSplitter;
class QDomElement;
class KToolBar;
class QBuffer;
class QFrame;
class FTStream;

#include "xmpp_chatstate.h"

#include "settings.h"

namespace XMPP 
{
	class Message;
	class FileTransfer;
}

namespace Phonon
{
	class ByteStream;
	class VideoPlayer;
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
	SessionView(QWidget *parent, const QString &jid, const int id);

	/**
	* Destructor
	*/
	virtual ~SessionView();

	QString session() { return m_session; }
	QString jid() { return m_jid; }
	int const id() { return m_id; }

	void createToolBar();
	void chatMessage(const Message &message); 
	void whiteboardMessage(const Message &message);
	void fileTransfer(FileTransfer *ft);

public slots:
	void sendClicked();
	void sendWhiteboard(const QDomElement &wb);
	void setMode(QAction *);
	void transferRead(const QByteArray &a);
	void actionSendFileTriggered();

signals:
	void sendMessage(const Message &);

private:
	QVBoxLayout *m_mainlayout;
	QVBoxLayout *m_chatlayout;
	QHBoxLayout *m_bottomlayout;

	KToolBar *m_wbtoolbar;

	QSplitter *m_mainSplitter;
	QSplitter *m_chatSplitter;

	QFrame *m_chatFrame;

	WbWidget *m_wbwidget;
	
	QTextEdit *m_chatoutput;
	QTextEdit *m_chatinput;

	QPushButton *m_sendmsg;
	
	QString m_session;
	QString m_jid;
	ChatState m_lastChatState;
	int m_id;


	// TODO: TEST ONLY!
	QBuffer *m_testbuffer;
	Phonon::VideoPlayer *player;
	int bytes;
	QByteArray *ba;
	MediaPlayer *mediap;
	FTStream *m_ftstream;
};

#endif // SESSIONVIEW_H
