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

class QPushButton;
class QListWidgetItem;

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
  
public slots:
  void createMUCClicked(bool toggled);
  void joinMUCClicked(bool toggled);
  void bookmarkedMUC(QListWidgetItem *item);
  void groupChatJoined(const Jid &);
  void groupChatLeft(const Jid &);
  void groupChatPresence(const Jid &, const Status &);
  void groupChatError(const Jid &, int, const QString &);
  
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
