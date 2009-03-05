/*
 * muccontrol.cpp
 *
 * GUI for managing MUCs
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#ifndef MUCCONTROL_H
#define MUCCONTROL_H

#include <QWidget>
#include "makneto.h"

class MUCUserList;
class SessionView;

class MUCControl : public QWidget
{
  Q_OBJECT
  MUCUserList *m_userlist;
  QLabel *m_roleLabel;
  MUCItem::Affiliation m_affiliation;
  MUCItem::Role m_role;
  SessionView *m_sessionView;
  Makneto *m_makneto;
  Jid m_jid;
  QString m_nick;
  bool m_connected;
  public:
    /** Constructor **/
    MUCControl(SessionView *parent, Makneto *makneto, const QString &jid, const QString &nick = QString());
    void doDisconnect(const Jid &jid);
    bool connected() { return m_connected; }
    void setConnected(bool connected);
    MUCItem::Role role() { return m_role; }
    MUCItem::Affiliation affiliation() { return m_affiliation; }
    QString nick() { return m_nick; }
  public slots:
    /** Set MUC user presence (status)
     * @p jid Jabber ID of room with nickname of affected user
     * @p status New status of affected user
     **/
    void setPresence(const Jid &jid, const Status &);
    
    // Methods are commented in connection.h
    
    void voiceGranted(const QString &nick, const QString &reason = QString());
    void voiceRevoked(const QString &nick, const QString &reason = QString());
    void voiceRequested(const QString &nick);
    void subjectChanged(const QString &subject);
    void userKicked(const QString &nick, const QString &reason, const QString &actor);
    void userBanned(const QString &nick, const QString &reason, const QString &actor);
    void userRemovedAsNonMember(const QString &nick);
    void membershipGranted(const Jid &userJid, const QString &reason);
    void membershipRevoked(const Jid &userJid, const QString &reason);
    void moderationGranted(const QString &nick, const QString &reason = QString());
    void moderationRevoked(const QString &nick, const QString &reason = QString());
    
    void _grantVoice();
    void _revokeVoice();
    void _requestVoice();
    void _subjectChange();
    void _kickUser();
    void _banUser();
    void _grantMembership();
    void _revokeMembership();
    void _grantModeration();
    void _revokeModeration();
    void _grantAdministration();
    void _revokeAdministration();
    void _grantOwnership();
    void _revokeOwnership();
    

  signals:
    void grantVoice(const Jid &jid, const QString &nick, const QString &reason = QString());
    void revokeVoice(const Jid &jid, const QString &nick, const QString &reason = QString());
    void requestVoice(const Jid &jid);
    void subjectChange(const Jid &jid, const QString &subject);
    void kickUser(const Jid &jid, const QString &nick, const QString &reason = QString());
    void banUser(const Jid &jid, const Jid &userJid, const QString &reason = QString());
    void grantMembership(const Jid &jid, const Jid &userJid, const QString &reason = QString());
    void revokeMembership(const Jid &jid, const Jid &userJid, const QString &reason = QString());
    void grantModeration(const Jid &jid, const QString &nick, const QString &reason = QString());
    void revokeModeration(const Jid &jid, const QString &nick, const QString &reason = QString());
    void grantAdministration(const Jid &jid, const Jid &userJid, const QString &reason = QString());
    void revokeAdministration(const Jid &jid, const Jid &userJid, const QString &reason = QString());
    void grantOwnership(const Jid &jid, const Jid &userJid, const QString &reason = QString());
    void revokeOwnership(const Jid &jid, const Jid &userJid, const QString &reason = QString());
    void groupChatDisconnect(const Jid &);
};

#endif
