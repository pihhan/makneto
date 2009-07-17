/*
 * muccontrol.cpp
 *
 * GUI for managing MUCs
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#ifndef MUCCONTROL_H
#define MUCCONTROL_H

#include <QtCore/QObject>

#include "makneto.h"

class MUCView;

class MUC;
class User
{
  public:
    QString nick;
    MUCItem::Affiliation aff;
    MUCItem::Role role;
    Status::Type status;
    Jid jid;
    MUC *muc;
};

Q_DECLARE_METATYPE(User *);

class MUC
{
  public:
    QString ownNick, room, server;
    User *me;
    QList<User *> users;
    Jid jid;
    bool connected;
};

Q_DECLARE_METATYPE(MUC *)

enum Actions { None = 0, GrantOwnership = 1, RevokeOwnership = 2, GrantAdmin = 4, RevokeAdmin = 8,
   GrantModeration = 16, RevokeModeration = 32, GrantMembership = 64, RevokeMembership = 128,
   Ban = 256, Kick = 512, GrantVoice = 1024, RevokeVoice = 2048 };


class MUCControl : public QObject
{
  Q_OBJECT
  private:
    Makneto *m_makneto;
    MUCView *m_view;
    QList<MUC *> m_mucs;
  public:
    MUCControl(Makneto *makneto);
    MUC *newMUC(const QString &s);
    MUC *getMUC(const QString &room, const QString &server);
    MUC *getMUC(const QString &roomJid);
    User *getUser(const QString &room, const QString &server, const QString &nick);
    User *getUser(const QString &roomJid, const QString &nick);

    int getUserActions(const QString &roomJid, const QString &nick);
    int getUserActions(User *me, User *user);

    void loadMUCs();
    void saveMUCs();

  public slots:
    void groupChatJoined(const Jid &jid);
    void groupChatLeft(const Jid &jid);
    void groupChatPresence(const Jid &jid, const Status &status);
    void groupChatError(const Jid &, int, const QString &message);
    void deleteMUC(MUC *);
    void deleteUser(User *user);

    void groupChatJoin(MUC *muc);
    void groupChatLeave(MUC *muc);
    /*
    void voiceGranted(const Jid &jid, const QString &nick, const QString &reason);
    void voiceRevoked(const Jid &jid, const QString &nick, const QString &reason);
    void voiceRequested(const Jid &jid, const QString &nick);
    void subjectChanged(const Jid &jid, const QString &subject);
    void userKicked(const Jid &jid, const QString &nick, const QString &reason, const QString &actor);
    void userBanned(const Jid &jid, const QString &nick, const QString &reason, const QString &actor);
    void userRemovedAsNonMember(const Jid &jid, const QString &nick);
    void membershipGranted(const Jid &jid, const Jid &userJid, const QString &reason);
    void membershipRevoked(const Jid &jid, const Jid &userJid, const QString &reason);
    void moderationGranted(const Jid &jid, const QString &nick, const QString &reason);
    void moderationRevoked(const Jid &jid, const QString &nick, const QString &reason);
    */
    void _grantVoice(const Jid &, const QString &, const QString &);
    void _revokeVoice(const Jid &, const QString &, const QString &);
    void _kickUser(const Jid &, const QString &, const QString &);
    void _banUser(const Jid &, const Jid &, const QString &);
    void _grantMembership(const Jid &, const Jid &, const QString &);
    void _revokeMembership(const Jid &, const Jid &, const QString &);
    void _grantModeration(const Jid &, const QString &, const QString &);
    void _revokeModeration(const Jid &, const QString &, const QString &);
    void _grantAdministration(const Jid &, const Jid &, const QString &);
    void _revokeAdministration(const Jid &, const Jid &, const QString &);
    void _grantOwnership(const Jid &, const Jid &, const QString &);
    void _revokeOwnership(const Jid &, const Jid &, const QString &);
  signals:
    void requestVoice(const Jid &);
    void subjectChange(const Jid &, const QString &);
    void grantVoice(const Jid &, const QString &, const QString &);
    void revokeVoice(const Jid &, const QString &, const QString &);
    void kickUser(const Jid &, const QString &, const QString &);
    void banUser(const Jid &, const Jid &, const QString &);
    void grantMembership(const Jid &, const Jid &, const QString &);
    void revokeMembership(const Jid &, const Jid &, const QString &);
    void grantModeration(const Jid &, const QString &, const QString &);
    void revokeModeration(const Jid &, const QString &, const QString &);
    void grantAdministration(const Jid &, const Jid &, const QString &);
    void revokeAdministration(const Jid &, const Jid &, const QString &);
    void grantOwnership(const Jid &, const Jid &, const QString &);
    void revokeOwnership(const Jid &, const Jid &, const QString &);
    void groupChatDisconnect(const Jid &);

    void addMUC(MUC *);
    void connectedToMUC(MUC *);
    void disconnectedFromMUC(MUC *);
    void error(MUC *, const QString &);
    void setUserStatus(User *);
    void deletedMUC(MUC *);
};

#endif
