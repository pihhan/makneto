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

#include <iostream>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>
#include <QtGui/QSplitter>
#include <QDomElement>
#include <QtCore/QSignalMapper>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtGui/QFrame>

#include "xmpp_message.h"
#include "xmpp_chatstate.h"
#include "xmpp_jid.h"
#include "filetransfer.h"

#include <Phonon/VideoPlayer>


SessionView::SessionView(QWidget *, const QString &jid, const int id): m_jid(jid), m_lastChatState(StateNone), m_id(id)
{
	m_mainSplitter = new QSplitter(this);
	m_mainSplitter->setOrientation(Qt::Vertical);

	m_chatFrame = new QFrame;

	m_chatSplitter = new QSplitter(m_chatFrame);
	m_chatSplitter->setOrientation(Qt::Vertical);

	m_mainlayout = new QVBoxLayout(this);
	m_mainlayout->setMargin(0);
	m_mainlayout->setSpacing(0);

	createToolBar();

	// add whiteboard widget
	m_wbwidget = new WbWidget("s1", "rezza@jabber.cz", QSize(300, 400), this);
	//m_mainlayout->addWidget(m_wbwidget, 1000);
	connect(m_wbwidget, SIGNAL(newWb(QDomElement)), SLOT(sendWhiteboard(QDomElement)));
	m_wbwidget->setMode(WbWidget::DrawPath);
	m_wbwidget->setMinimumSize(300, 400);

	m_chatlayout = new QVBoxLayout();

	m_chatoutput = new QTextEdit(this);
	m_chatinput = new QTextEdit(this);
	m_sendmsg = new QPushButton("&Send", this);
	connect(m_sendmsg, SIGNAL(clicked()), this, SLOT(sendClicked()));

	// output chat text edit props
	//m_chatoutput->setTextFormat(Qt::RichText); // FIXME: port to correct behaviour on Qt4
	m_chatoutput->setReadOnly(true);

	m_chatSplitter->addWidget(m_chatoutput);
	m_chatSplitter->addWidget(m_chatinput);
	m_chatSplitter->addWidget(m_sendmsg);


	m_mainSplitter->addWidget(m_wbwidget);
	m_mainSplitter->addWidget(m_chatSplitter);

	m_mainlayout->addWidget(m_mainSplitter);
	m_mainlayout->addWidget(m_chatFrame);

	setLayout(m_mainlayout);

	// TODO: test only!!!
	ba = new QByteArray;
	m_testbuffer = new QBuffer(ba, this);
	m_testbuffer->open(QIODevice::ReadWrite);

}

SessionView::~SessionView()
{

}

void SessionView::createToolBar()
{
	// wb toolbar
	m_wbtoolbar = new KToolBar(this);
	m_wbtoolbar->setIconDimensions(16);
	m_wbtoolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

	m_mainlayout->addWidget(m_wbtoolbar);

	QActionGroup *groupMode = new QActionGroup(this);
	connect(groupMode, SIGNAL(triggered(QAction*)), this, SLOT(setMode(QAction*)));
 
	KAction *actionSelect = new KAction(KIcon("select-rectangular"), i18n("Selection"), groupMode);
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
// 	KAction *actionText = new KAction(KIcon("insert-text"), i18n("Text"), groupMode);
// 	m_wbtoolbar->addAction(actionText);
// 	actionText->setData(QVariant(WbWidget::DrawText));

	KAction *actionImage = new KAction(KIcon("insert-image"), i18n("Image"), groupMode);
	m_wbtoolbar->addAction(actionImage);
	actionImage->setData(QVariant(WbWidget::DrawImage));

	KAction *actionSendFile = new KAction(KIcon("mail-send"), i18n("Send file"), this);
	m_wbtoolbar->addAction(actionSendFile);
	connect(actionSendFile, SIGNAL(triggered()), this, SLOT(actionSendFileTriggered()));


}

void SessionView::sendClicked()
{
	QString text;
	Message message;

	// prepare message
	message.setTo(m_jid);
	message.setFrom(Settings::jabberID()+"/Makneto");
	message.setType("chat");
	message.setChatState(StateActive);
        // TODO: support xhtml using correct xep, for now, only plaintext
	message.setBody(m_chatinput->toPlainText());

	emit sendMessage(message);

	// format for chat window
	text = "<b>Me</b>: "+m_chatinput->toPlainText();

	m_chatinput->setText("");

	// show text in chat window
	m_chatoutput->append(text);
}

void SessionView::sendWhiteboard(const QDomElement &wb)
{
	Message message;

	message.setTo(m_jid);
	message.setWhiteboard(wb);

	emit sendMessage(message);
}

void SessionView::chatMessage(const Message &message)
{
	QString text;

	switch (message.chatState())
	{
		case StateComposing:
			if (m_lastChatState != StateComposing)
				text += "<i>"+message.from().bare()+" is composing message</i>";
			else
				return;
			break;

		case StateGone:
			text += "<i>"+message.from().bare()+" has gone</i>";
			break;

		case StateInactive:
		case StatePaused:
		case StateNone:
		case StateActive:
		default:
			if (!message.body().isEmpty())
				text = "<b>"+message.from().bare()+"</b>: "+message.body();
			else
			{
				m_lastChatState = message.chatState();
				return;
			}
	}

	m_chatoutput->append(text);

	m_lastChatState = message.chatState();
}

void SessionView::whiteboardMessage(const Message &message)
{
	m_wbwidget->processWb(message.whiteboard());
}

void SessionView::fileTransfer(FileTransfer *ft)
{
	FileTransfer *transfer = ft;

	QString text;

	text = "<b>" + transfer->peer().bare()+"</b> Incoming file '" + transfer->fileName() +"'";

	m_chatoutput->append(text);

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

void SessionView::setMode(QAction *action)
{
	m_wbwidget->setMode(WbWidget::Mode(action->data().toInt()));
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

void SessionView::actionSendFileTriggered()
{

}
