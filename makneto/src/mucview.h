/*
 * mucview.h
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#ifndef MUCVIEW_H
#define MUCVIEW_H

#include <QtGui/QWidget>

#include "makneto.h"

class QTreeWidget;
class QTreeWidgetItem;

class MUCControl;

class MUC;
class User;

/*! \brief Display of list of rooms, and their participants. */
class MUCView : public QWidget
{
  Q_OBJECT
  private:
    Makneto *m_makneto;
    MaknetoView *m_maknetoview;
    MUCControl *m_control;
    QTreeWidget *tree;
    KIcon iconOffline[3], iconOnline[3], iconAway[3], iconXA[3], iconDND[3], iconInvisible[3], iconFFC[3];
    void createIcons();
    KIcon getColorizedIcon(const KIcon &icon, QColor color);
  public:
    MUCView(MaknetoView *view, Makneto *makneto);

    KIcon getIcon(const Status &status, const MUCItem::Role &role);
    bool getConferenceSetting(QString &room, QString &server, QString &nick);

    QTreeWidgetItem *getMUCItem(MUC *muc);
    QTreeWidgetItem *getUserItem(User *user);
    bool isMUCConnected(MUC *muc);
    bool isMUCConnected(const Jid &jid);    
  public slots:
    void addMUC(MUC *muc);
    void joinMUC();
    void itemDoubleClicked(QTreeWidgetItem *, int);
    void contextMenu(const QPoint &);
    void connectedToMUC(MUC *muc);
    void disconnectedFromMUC(MUC *muc);
    void setUserStatus(User *user);
    void error(MUC *muc, const QString &message);
    void deletedMUC(MUC *muc);
    void showInfoMessage(MUC *muc, const QString &message);
  signals:
    void connectToMUC(MUC *);
    void disconnectFromMUC(MUC *);
    void deleteMUC(MUC *);
};

#endif // MUCVIEW_H
