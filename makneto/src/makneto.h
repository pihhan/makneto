/*
 * makneto.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETO_H
#define MAKNETO_H

#include "maknetomainwindow.h"
#include "connection.h"
#include "featurelist.h"

#include <QObject>

namespace XMPP 
{
	class Message;
	class Status;
}

using namespace XMPP;

/**
 * This is Makneto class - central class of Makneto suite
 *
 * @short Makneto
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class MaknetoContactList;
class MaknetoMainWindow;
class MUCControl;
class MediaManager;

enum ChatType { Chat = 0, GroupChat = 1 };

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

	Connection *getConnection() { return m_conn; }
	MaknetoContactList *getContactList() { return m_cl; }
  MaknetoMainWindow *getMaknetoMainWindow() { return m_mainwindow; }
        FeatureListManager *getFeatureManager() { return m_flm; }

        MUCControl  *getMUCControl() const;
        void        setMUCControl(MUCControl *muc);

        MediaManager *getMediaManager() const;

public slots:
	void conn_messageReceived(const Message &);
// 	void conn_statusChanged(const XMPP::Status &);
  void actionNewSession(const QString &, ChatType type = Chat, const QString &nick = QString());
	void actionNewSession();
  void contactNewSession(QAction *action);
  void contactDetails(QAction *action);
	void addUser(const XMPP::Jid &, const QString &, bool requestAuth);
  
  void setMaknetoMainWindow(MaknetoMainWindow *mainwindow) { m_mainwindow = mainwindow; }

signals:
  void newSession(const QString &text, ChatType type, const QString &nick = QString());
  void statusChanged(const XMPP::Status &);

  
private:
    Connection *m_conn;
    MaknetoContactList *m_cl;
    MaknetoMainWindow *m_mainwindow;
    FeatureListManager *m_flm;
    MUCControl  *m_muccontrol;
    MediaManager    *m_mediamgr;
};

#endif // MAKNETO_H
