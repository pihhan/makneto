/*
 * sessionview.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QtGui/QWidget>
#include <QtCore/QByteArray>

#include "mediaplayer.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTextEdit;
class WbWidget;
class QSplitter;
class QDomElement;
class KToolBar;
class QBuffer;
class QFrame;
class FTStream;
class Makneto;
class PaletteWidget;
class ChatOutput;

#include <xmpp_chatstate.h>

#include "settings.h"
#include "wbwidget.h"
#include <ktabwidget.h>

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
 * @version 0.2
 */

class SessionView : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
  * @param type Type of the session 0 = Chat, 1 = MUC
	*/
  SessionView(QWidget *parent, const QString &jid, const int id, int type = 0, const QString &nick = QString());

	/**
	* Destructor
	*/
	virtual ~SessionView();

	QString session();
	QString jid();
  int id() const;
  
	void createToolBar();
    void configureWhiteboardTab();
    void configureChatInput();
    void configureMediaTab();

    void createDrawingTab();
	void chatMessage(const Message &message);
  void infoMessage(const QString &text);
	void whiteboardMessage(const Message &message);
	void fileTransfer(FileTransfer *ft);

  int type(void);
  void setType(int type);
  
  //MUCControl *getMUCControl(void) { return m_muccontrol; }
  void showHideChat();
  bool closeRequest();
  void setEnabled(bool enabled);
public slots:
  void sendClicked();
  void sendWhiteboard(const QDomElement &wb);
  void setMode(QAction *);
  void transferRead(const QByteArray &a);
  void actionSendFileTriggered();
  void actionCreatePollTriggered();
  void fgColorChanged(const QColor &c);
  void bgColorChanged(const QColor &c);
  void penSizeChanged(int size);
  void modeChanged(WbWidget::Mode);

  Makneto *makneto() const;

signals:
	void sendMessage(const Message &);

private:
  Makneto *m_makneto;
  QVBoxLayout *m_topLayout;
  QWidget *m_leftWidget, *m_chatWidget;
  PaletteWidget *m_paletteWidget;
  QVBoxLayout *m_leftLayout;
  QSplitter *m_leftSplitter;
  QSplitter *m_topSplitter;
  QStringList messages;
   
  QVBoxLayout *m_mainlayout;
  QVBoxLayout *m_chatlayout;
  QHBoxLayout *m_bottomlayout;

  KToolBar *m_wbtoolbar;

  QSplitter *m_mainSplitter;
  QSplitter *m_chatSplitter;

  QFrame *m_chatFrame;

  WbWidget *m_wbwidget;
	
  ChatOutput *m_chatoutput;
  QTextEdit *m_chatinput;

  QPushButton *m_sendmsg;
  
  QString m_jid;
  ChatState m_lastChatState;
  int m_id;
  QString m_session;
  int m_type;
  QString m_nick;
  KAction *actionSelect;
  KTabWidget *m_tabs;


	// TODO: TEST ONLY!
	QBuffer *m_testbuffer;
	Phonon::VideoPlayer *player;
	int bytes;
	QByteArray *ba;
	MediaPlayer *mediap;
	FTStream *m_ftstream;
};

#endif // SESSIONVIEW_H
