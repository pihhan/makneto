/*
 * mucview.h
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#ifndef MUCVIEW_H
#define MUCVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QMetaType>

#include "makneto.h"
#include "maknetoview.h"

class QPushButton;
class QListWidgetItem;
class SessionView;

class tMUC
{
  public:
    QString server, room, nick;
};

Q_DECLARE_METATYPE(tMUC *)

class MUCView : public QWidget
{
  Q_OBJECT
public:
  /**
   * Default constructor
   */
  MUCView(QWidget *parent, Makneto *makneto);

  /**
   * Destructor
   */
  virtual ~MUCView();
  
  void loadBookmarks(void);
  void saveBookmarks(void);
  SessionView *getSessionByJid(const Jid &);
  
public slots:
  void createMUCClicked(bool toggled);
  void joinMUCClicked(bool toggled);
  void bookmarkedMUC(QListWidgetItem *item);
  void groupChatJoined(const Jid &);
  void groupChatLeft(const Jid &);
  void groupChatPresence(const Jid &, const Status &);
  void groupChatError(const Jid &, int, const QString &);
  
  void voiceGranted(const Jid &jid, const QString &nick, const QString &reason = QString());
  void voiceRevoked(const Jid &jid, const QString &nick, const QString &reason = QString());
  void voiceRequested(const Jid &jid, const QString &nick);
  void subjectChanged(const Jid &jid, const QString &subject);
  void userKicked(const Jid &jid, const QString &nick, const QString &reason, const QString &actor);
  void userBanned(const Jid &jid, const QString &nick, const QString &reason, const QString &actor);
  void userRemovedAsNonMember(const Jid &jid, const QString &nick);
  void membershipGranted(const Jid &jid, const Jid &userJid, const QString &reason);
  void membershipRevoked(const Jid &jid, const Jid &userJid, const QString &reason);
  void moderationGranted(const Jid &jid, const QString &nick, const QString &reason = QString());
  void moderationRevoked(const Jid &jid, const QString &nick, const QString &reason = QString());
  
private:
  QVBoxLayout *m_mainlayout;
  QHBoxLayout *m_buttonslayout;

  QPushButton *m_createMUC;
  QPushButton *m_joinMUC;
  
  QListWidget *m_MUCbookmarks;

  Makneto *m_makneto;
  
  bool getConferenceSetting(QString &room, QString &server, QString &nick);
};

#endif // MUCVIEW_H
