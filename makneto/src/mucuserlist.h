/*
 * mucuserlist.h
 *
 * Header for user list widget
 *
 * Copyright (C) 2009 Radek Novacek <rad.n@centrum.cz>
 */

#ifndef MUCUSERLIST_H
#define MUCUSERLIST_H

#include <QListWidget>
#include "makneto.h"

class MUCControl;

// enum Affiliation { NoChangeAffiliation = -1, UnknownAffiliation, Outcast, NoAffiliation, Member, Admin, Owner };
// enum Role { NoChangeRole = -1, UnknownRole, NoRole, Visitor, Participant, Moderator };
// enum UserStatus { NoChangeStatus = -1, Offline, Online, Away, XA, DND, Invisible, FFC };

struct User
{
  Jid jid;
  QString nick;
  MUCItem::Affiliation affiliation;
  MUCItem::Role role;
  Status::Type status;
  QListWidgetItem *item;
  void update();
  User(const QString &_nick, const Jid &_jid = Jid(), const MUCItem::Affiliation &_affiliation = MUCItem::UnknownAffiliation,
       const MUCItem::Role &_role = MUCItem::UnknownRole, const Status::Type &_status = Status::Online, QListWidget *listWidget = 0);
};

Q_DECLARE_METATYPE(User *);

class MUCUserList : public QListWidget
{
  private:
    QList<User *> users;
    MUCControl *m_mucControl;
    QMenu *popupMenu;

    QAction *grantVoice, *revokeVoice, *kickUser, *banUser;
    QAction *grantMembership, *revokeMembership, *grantModeration, *revokeModeration;
    QAction *grantOwnership, *revokeOwnership, *grantAdministration, *revokeAdministration;
    QString m_affectedUser;
    Jid m_affectedUserJid;
    
    User *findUser(const QString &nick);
    void contextMenuEvent(QContextMenuEvent *event);
  public:
    MUCUserList(MUCControl *parent);
    /** Updates information about user - adds him when his nick not exist, removes him when status = Offline, changes info otherwise
     * @p nick Nick of the user
     * @p jid Jabber ID of the user
     * @p affiliation Affiliation to the room - NoChangeAffiliation means it will not change
     * @p role Role to the room - NoChangeRole means it will not change
     * @p status Status of the user - Status() means no change
     **/
    void updateUser(const QString &nick, const Jid &jid = Jid(), const MUCItem::Affiliation &affiliation = MUCItem::UnknownAffiliation,
                    const MUCItem::Role &role = MUCItem::UnknownRole, const Status::Type &status = Status::Online);
    bool renameUser(const QString &nick, const QString &newNick);
    QString affectedUser() { return m_affectedUser; }
    Jid affectedUserJid() { return m_affectedUserJid; }
};

#endif
