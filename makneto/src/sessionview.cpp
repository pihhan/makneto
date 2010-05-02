/*
 * sessionview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "sessionview.h"
#include "mediaplayer.h"
#include "kiconloader.h"
#include "klocale.h"
#include "wbwidget.h"
#include "ktoolbar.h"
#include "kaction.h"
#include "ftstream.h"
#include "maknetoview.h"
#include "mucview.h"

#include "mediaviewwidget.h"

#include <iostream>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>
#include <QtGui/QSplitter>
#include <QtXml/QDomElement>
#include <QtCore/QSignalMapper>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtGui/QFrame>
#include <QtCore/QDebug>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtGui/QGraphicsProxyWidget>

#include "xmpp_message.h"
#include "xmpp_chatstate.h"
#include "xmpp_jid.h"
#include "filetransfer.h"
#include "sessiontabmanager.h"
#include "makneto.h"
#include "chatinput.h"
#include "chatoutput.h"
#include "palettewidget.h"
#include "plugins/pollplugin.h"
#include "wbforeign.h"

#include <Phonon/VideoPlayer>


SessionView::SessionView(QWidget *parent, const QString &jid, const int id, int type, const QString &nick)
  : m_jid(jid), m_lastChatState(StateNone), m_id(id), m_type(type), m_nick(nick)
{
  m_makneto = (dynamic_cast<SessionTabManager *> (parent))->makneto();
  m_topLayout = new QVBoxLayout(this);
  m_topSplitter = new QSplitter(this);
  m_topLayout->addWidget(m_topSplitter);
  m_topSplitter->setOrientation(Qt::Horizontal);

  m_tabs = new KTabWidget();
  
  configureWhiteboardTab();
  configureChatInput();
  configureMediaTab();

  m_topSplitter->addWidget(m_tabs);
  setLayout(m_topLayout);  

	// TODO: test only!!!
	ba = new QByteArray;
	m_testbuffer = new QBuffer(ba, this);
	m_testbuffer->open(QIODevice::ReadWrite);

}

SessionView::~SessionView()
{
  QSettings settings;
  settings.setValue("m_topSplitter", m_topSplitter->saveState());
}

/*! \brief Create whiteboard tab. */
void SessionView::configureWhiteboardTab()
{
  m_leftWidget = new QWidget(this);
  
  m_leftLayout = new QVBoxLayout(m_leftWidget);
  m_leftWidget->setLayout(m_leftLayout);
  
  m_leftLayout->setMargin(0);
  m_leftLayout->setSpacing(0);
  
  // Add Toolbar to the layout
  createToolBar();
  QHBoxLayout *m_toolLayout = new QHBoxLayout();
  m_toolLayout->addWidget(m_wbtoolbar);

  // Create palette widget
  m_paletteWidget = new PaletteWidget(m_leftWidget);
  connect(m_paletteWidget, SIGNAL(fgColorChanged(const QColor &)), SLOT(fgColorChanged(const QColor &)));
  connect(m_paletteWidget, SIGNAL(bgColorChanged(const QColor &)), SLOT(bgColorChanged(const QColor &)));
  connect(m_paletteWidget, SIGNAL(penSizeChanged(int)), SLOT(penSizeChanged(int)));
  m_toolLayout->addWidget(m_paletteWidget);

  m_leftLayout->addLayout(m_toolLayout);

  // Create WhiteBoard widget
  m_wbwidget = new WbWidget("s1", m_makneto->getConnection()->jid().bare(), QSize(300, 400), this);
  connect(m_wbwidget, SIGNAL(newWb(QDomElement)), SLOT(sendWhiteboard(QDomElement)));
  connect(m_wbwidget, SIGNAL(modeChanged(WbWidget::Mode)), SLOT(modeChanged(WbWidget::Mode)));
  m_wbwidget->setMode(WbWidget::DrawPath);
  m_wbwidget->setMinimumSize(300, 400);
  //m_topSplitter->addWidget(m_leftWidget);
 
  m_paletteWidget->setFgColor(QColor(0, 0, 0));
  m_paletteWidget->setBgColor(Qt::transparent);
  m_paletteWidget->setPenSize(1);

  m_leftSplitter = new QSplitter(Qt::Vertical);
  m_leftSplitter->addWidget(m_wbwidget);
  m_leftLayout->addWidget(m_leftSplitter);

  m_tabs->addTab(m_leftWidget, KIcon("draw-freehand"), tr("Whiteboard"));
}

/*! \brief Create tab with text message input. */
void SessionView::configureChatInput()
{
  QWidget *chattab = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout();

  m_chatSplitter = new QSplitter(Qt::Vertical, m_leftSplitter);
  
  layout->addWidget(m_chatSplitter);

  m_chatoutput = new ChatOutput(m_chatSplitter);
  
  m_chatinput = new ChatInput(m_chatSplitter);
  m_sendmsg = new QPushButton(tr("&Send"), m_leftWidget);
  connect(m_sendmsg, SIGNAL(clicked()), this, SLOT(sendClicked()));
  connect(m_chatinput, SIGNAL(send()), this, SLOT(sendClicked()));

  // output chat text edit props
  m_chatoutput->setReadOnly(true);
  m_chatoutput->setNick(m_nick);
  
  m_chatSplitter->addWidget(m_chatoutput);
  m_chatSplitter->addWidget(m_chatinput);
  int inputpos = m_chatSplitter->indexOf(m_chatoutput);
  m_chatSplitter->setStretchFactor(inputpos, 30);
  
  layout->addWidget(m_sendmsg);
  chattab->setLayout(layout);

  m_tabs->addTab(chattab, KIcon("document-edit"), tr("Messages"));
  //m_leftLayout->addLayout(layout);

}

void SessionView::configureMediaTab()
{
    QWidget *m_mediaw = new MediaViewWidget(this);
    m_tabs->addTab(m_mediaw, KIcon("voicecall"), tr("Media"));
}

void SessionView::createDrawingTab()
{
    
}

void SessionView::createToolBar()
{
	// wb toolbar
	m_wbtoolbar = new KToolBar(this);
	m_wbtoolbar->setIconDimensions(16);
	m_wbtoolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

	QActionGroup *groupMode = new QActionGroup(this);
	connect(groupMode, SIGNAL(triggered(QAction*)), this, SLOT(setMode(QAction*)));
 
	actionSelect = new KAction(KIcon("select-rectangular"), i18n("Selection"), groupMode);
	m_wbtoolbar->addAction(actionSelect);
	actionSelect->setData(QVariant(WbWidget::Select));

	KAction *actionTransformMove = new KAction(KIcon("transform-move"), i18n("Transform - move"), groupMode);
	m_wbtoolbar->addAction(actionTransformMove);
	actionTransformMove->setData(QVariant(WbWidget::Translate));

	KAction *actionRotate = new KAction(KIcon("transform-rotate"), i18n("Transform - rotate"), groupMode);
	m_wbtoolbar->addAction(actionRotate);
	actionRotate->setData(QVariant(WbWidget::Rotate));

	KAction *actionScale = new KAction(KIcon("zoom-fit-best"), i18n("Transform - scale"), groupMode);
	m_wbtoolbar->addAction(actionScale);
	actionScale->setData(QVariant(WbWidget::Scale));

	KAction *actionErase = new KAction(KIcon("draw-eraser"), i18n("Erase"), groupMode);
	m_wbtoolbar->addAction(actionErase);
	actionErase->setData(QVariant(WbWidget::Erase));

	KAction *actionPencil = new KAction(KIcon("draw-freehand"), i18n("Freehand"), groupMode);
	m_wbtoolbar->addAction(actionPencil);
	actionPencil->setData(QVariant(WbWidget::DrawPath));

	KAction *actionLine = new KAction(KIcon("draw-line"), i18n("Line"), groupMode);
	m_wbtoolbar->addAction(actionLine);
	actionLine->setData(QVariant(WbWidget::DrawLine));

	KAction *actionRectangle = new KAction(KIcon("draw-rectangle"), i18n("Rectangle"), groupMode);
	m_wbtoolbar->addAction(actionRectangle);
	actionRectangle->setData(QVariant(WbWidget::DrawRectangle));

	KAction *actionEllipse = new KAction(KIcon("draw-ellipse"), i18n("Ellipse"), groupMode);
	m_wbtoolbar->addAction(actionEllipse);
	actionEllipse->setData(QVariant(WbWidget::DrawEllipse));

	KAction *actionCircle = new KAction(KIcon("draw-circle"), i18n("Circle"), groupMode);
	m_wbtoolbar->addAction(actionCircle);
	actionCircle->setData(QVariant(WbWidget::DrawCircle));

// 	KAction *actionPolyline = new KAction(KIcon("draw-polyline"), i18n("Polyline"), groupMode);
// 	m_wbtoolbar->addAction(actionPolyline);
// 	actionPolyline->setData(QVariant(WbWidget::DrawPolyline));
// 
  KAction *actionText = new KAction(KIcon("insert-text"), i18n("Text"), groupMode);
  m_wbtoolbar->addAction(actionText);
  actionText->setData(QVariant(WbWidget::DrawText));
  actionText->setCheckable(true);

	KAction *actionImage = new KAction(KIcon("insert-image"), i18n("Image"), groupMode);
	m_wbtoolbar->addAction(actionImage);
	actionImage->setData(QVariant(WbWidget::DrawImage));

	KAction *actionSendFile = new KAction(KIcon("mail-send"), i18n("Send file"), this);
	m_wbtoolbar->addAction(actionSendFile);
	connect(actionSendFile, SIGNAL(triggered()), this, SLOT(actionSendFileTriggered()));

  KAction *actionPoll = new KAction(KIcon("maknetopoll"), i18n("Create poll"), this);
  m_wbtoolbar->addAction(actionPoll);
  connect(actionPoll, SIGNAL(triggered()), this, SLOT(actionCreatePollTriggered()));
}

void SessionView::sendClicked()
{
	QString text;
	Message message;

	// prepare message
	message.setTo(m_jid);
  if (type() == 0)
    message.setType("chat");
  else
    message.setType("groupchat");
  message.setChatState(StateActive);
  // TODO: support xhtml using correct xep, for now, only plaintext
  message.setBody(m_chatinput->toPlainText());

  emit sendMessage(message);
  
  // Show message in chat window only in chat, not in groupchat
  if (type() == 0)
  {
    // show text in chat window
  }
  
  m_chatinput->setText("");
}

void SessionView::sendWhiteboard(const QDomElement &wb)
{
	Message message;

	message.setTo(m_jid);
	message.setTo(m_jid);
  if (type() == 0)
    message.setType("chat");
  else
    message.setType("groupchat");
  QString id = m_makneto->getConnection()->genUniqueId();
  message.setId(id);
	message.setWhiteboard(wb);

	emit sendMessage(message);
}

void SessionView::chatMessage(const Message &message)
{
  int type = 0; // Type of the message (0 = message, 1 = information)
	QString text;
	switch (message.chatState())
	{
		case StateComposing:
			if (m_lastChatState != StateComposing)
      {
        if (message.type().compare(QString("groupchat"), Qt::CaseInsensitive) == 0) // Is it a MUC ?
          text = message.from().resource() + " is composing message";
        else
          text = message.from().bare() + " is composing message";
        type = 1;
      }
			else
				return;
			break;

		case StateGone:
      if (message.type().compare(QString("groupchat"), Qt::CaseInsensitive) == 0)
        text = message.from().resource() + " has gone";
      else
        text = message.from().bare() + " has gone";
      type = 1;
			break;

		case StateInactive:
		case StatePaused:
		case StateNone:
		case StateActive:
		default:
			if (!message.body().isEmpty())
      {
        if (message.type().compare(QString("groupchat"), Qt::CaseInsensitive) == 0) // Is it a MUC ?
          text = "<b>"+message.from().resource()+"</b>: "+message.body();
        else
          text = "<b>"+message.from().bare()+"</b>: "+message.body();
        type = 0;
      }
			else
			{
				m_lastChatState = message.chatState();
				return;
			}
	}

  if (message.type() == "error") {
    m_chatoutput->errorMessage(QString("%1 #%2")
        .arg(message.error().text)
        .arg(message.error().type));
  } else if (type == 0)
    m_chatoutput->incomingMessage(text);
  else
    m_chatoutput->infoMessage(text);

	m_lastChatState = message.chatState();
}

void SessionView::infoMessage(const QString &text)
{
  m_chatoutput->infoMessage(text);
}

void SessionView::whiteboardMessage(const Message &message)
{
  // During MUC when you send message you will obtain the same message from server - so if I'm the sender, we will throw it away...
  if (message.from().resource().compare(m_nick))
    m_wbwidget->processWb(message.whiteboard());
}

void SessionView::fileTransfer(FileTransfer *ft)
{
	FileTransfer *transfer = ft;

	QString text;

	text = "<b>" + transfer->peer().bare()+"</b> Incoming file '" + transfer->fileName() +"'";

  m_chatoutput->incomingMessage(text);

	connect(transfer, SIGNAL(readyRead(const QByteArray &)), this, SLOT(transferRead(const QByteArray &)));

	transfer->accept();

	m_ftstream = new FTStream(m_testbuffer, transfer->fileSize(), this);

	mediap = new MediaPlayer(this);
	mediap->show();
	mediap->setCurrentSource(m_ftstream);
	
	//mediap->setBuffer(m_testbuffer);

	bytes = 0;
	//player->load(m_testbuffer);
	//player->play();
}

void SessionView::showHideChat()
{
  QList<int> l = m_leftSplitter->sizes();
  if (l[1] == 0)
  {
    QSettings settings;
    l[1] = settings.value("m_leftSplitter_size").toInt();
  }
  else
  {
    QSettings settings;
    settings.setValue("m_leftSplitter_size", l[1]);
    l[1] = 0;
  }
  m_leftSplitter->setSizes(l);
}

bool SessionView::closeRequest()
{
  if (m_type == 1) // Is it a groupchat ?
  {
    if (m_makneto->getMaknetoMainWindow()->getMaknetoView()->getMUCView()->isMUCConnected(m_jid))
    {
      if (QMessageBox::question(this, "Makneto",
                              tr("Do you really want to close the tab?\nYou won't be able to recieve new messages from this conference!"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
      {
        m_makneto->getConnection()->groupChatLeave(m_jid);
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return true;
}

void SessionView::setEnabled(bool enabled)
{
  m_sendmsg->setEnabled(enabled);
}

void SessionView::setMode(QAction *action)
{
	m_wbwidget->setMode(WbWidget::Mode(action->data().toInt()));
  // If user selects freehand pen, we should disable the brush
  if (WbWidget::Mode(action->data().toInt()) == WbWidget::DrawPath)
    m_wbwidget->setFillColor(Qt::transparent);
  else
    m_wbwidget->setFillColor(m_paletteWidget->bgColor());
}

void SessionView::modeChanged(WbWidget::Mode mode)
{
  switch (mode)
  {
    case WbWidget::Select:
      actionSelect->setChecked(true);
      break;
    case WbWidget::DrawText:
      //actionDrawText->setChecked(true);
      break;
    default:
      break;
  }
}


void SessionView::transferRead(const QByteArray &a)
{
	ba->append(a);
/*	std::cout << "a.size=" << a.size() << std::endl;*/
	bytes=bytes+a.size();

/*	std::cout << "written=" << m_testbuffer->write(a.data()) << std::endl;
	m_testbuffer->waitForBytesWritten(1000);*/
 //	std::cout << "buffer.size()=" << m_testbuffer->size() << std::endl;
// 	std::cout << "bytes=" << bytes << std::endl;
// 	std::cout << "ba.size()=" << ba->size() << std::endl;

// 	if (bytes==183173120)
// 	{
// 		m_testbuffer->open(QIODevice::ReadOnly);
// 		//m_testbuffer->seek(0);
// 		mediap->setCurrentSource(m_testbuffer);
// 		mediap->show();
// 	}

 	if (bytes==4456448)
		mediap->setCurrentSource(m_ftstream);

}

void SessionView::fgColorChanged(const QColor &c)
{
  m_wbwidget->setStrokeColor(c);
}

void SessionView::bgColorChanged(const QColor &c)
{
  m_wbwidget->setFillColor(c);
}

void SessionView::penSizeChanged(int size)
{
  m_wbwidget->setStrokeWidth(size);
}

void SessionView::actionSendFileTriggered()
{

}

void SessionView::actionCreatePollTriggered()
{
  QDomElement dom;  
  PollPlugin *plugin = new PollPlugin();
  if (!plugin->getQuestions())
    return;

  WbForeign *wbf = new WbForeign(plugin, dom, m_wbwidget->scene->newId(), m_wbwidget->scene->newIndex(), QString("root"), static_cast<QGraphicsScene *> (m_wbwidget->scene));
  //scene->update(wbf->graphicsItem()->boundingRect());
  m_wbwidget->scene->queueNew(wbf->id(), wbf->index(), wbf->svg());
  m_wbwidget->scene->addWbItem(wbf);
  plugin->graphicsItem()->ensureVisible(QRectF(), 0, 0);
  setMode(actionSelect);
  actionSelect->setChecked(true);
}

QString SessionView::session() 
{ 
    return m_session; 
}

QString SessionView::jid() 
{ 
    return m_jid; 
}
int SessionView::id() const 
{ 
    return m_id; 
}

int SessionView::type(void) 
{ 
    return m_type; 
}

void SessionView::setType(int type)
{ 
    m_type = type; 
}
  
  
Makneto *SessionView::makneto() const
{
    return m_makneto;
}

