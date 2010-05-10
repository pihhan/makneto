
#include "muccontrol.h"

#include "mucview.h"

MUCControl::MUCControl(Makneto *makneto) : m_makneto(makneto)
{
  makneto->setMUCControl(this);
  connect(makneto->getConnection(), SIGNAL(groupChatJoined(const Jid &)), SLOT(groupChatJoined(const Jid &)));
  connect(makneto->getConnection(), SIGNAL(groupChatLeft(const Jid &)), SLOT(groupChatLeft(const Jid &)));
  connect(makneto->getConnection(), SIGNAL(groupChatPresence(const Jid &, const Status &)), SLOT(groupChatPresence(const Jid &, const Status &)));
  connect(makneto->getConnection(), SIGNAL(groupChatError(const Jid &, int, const QString &)), SLOT(groupChatError(const Jid &, int, const QString &)));

  connect(this, SIGNAL(grantVoice(const Jid &, const QString &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatGrantVoice(const Jid &, const QString &, const QString &)));
  connect(this, SIGNAL(revokeVoice(const Jid &, const QString &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatRevokeVoice(const Jid &, const QString &, const QString &)));
  connect(this, SIGNAL(requestVoice(const Jid &)),
          m_makneto->getConnection(), SLOT(groupChatRequestVoice(const Jid &)));
  connect(this, SIGNAL(subjectChange(const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatSubjectChange(const Jid &, const QString &)));
  connect(this, SIGNAL(kickUser(const Jid &, const QString &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatKickUser(const Jid &, const QString &, const QString &)));
  connect(this, SIGNAL(banUser(const Jid &, const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatBanUser(const Jid &, const Jid &, const QString &)));
  connect(this, SIGNAL(grantMembership(const Jid &, const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatGrantMembership(const Jid &, const Jid &, const QString &)));
  connect(this, SIGNAL(revokeMembership(const Jid &, const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatRevokeMembership(const Jid &, const Jid &, const QString &)));
  connect(this, SIGNAL(grantModeration(const Jid &, const QString &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatGrantModeration(const Jid &, const QString &, const QString &)));
  connect(this, SIGNAL(revokeModeration(const Jid &, const QString &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatRevokeModeration(const Jid &, const QString &, const QString &)));
  connect(this, SIGNAL(grantAdministration(const Jid &, const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatGrantAdministration(const Jid &, const Jid &, const QString &)));
  connect(this, SIGNAL(revokeAdministration(const Jid &, const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatRevokeAdministration(const Jid &, const Jid &, const QString &)));
  connect(this, SIGNAL(grantOwnership(const Jid &, const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatGrantOwnership(const Jid &, const Jid &, const QString &)));
  connect(this, SIGNAL(revokeOwnership(const Jid &, const Jid &, const QString &)),
          m_makneto->getConnection(), SLOT(groupChatRevokeOwnership(const Jid &, const Jid &, const QString &)));
  connect(this, SIGNAL(groupChatDisconnect(const Jid &)),
          m_makneto->getConnection(), SLOT(groupChatLeave(const Jid &)));
}

MUC *MUCControl::newMUC(const QString &s)
{
  MUC *muc = new MUC;
  muc->jid = Jid(s);
  muc->room = muc->jid.node();
  muc->server = muc->jid.domain();
  muc->ownNick = muc->jid.resource();
  muc->connected = false;
  m_mucs << muc;
  emit addMUC(muc);
  return muc;
}

MUC *MUCControl::getMUC(const QString &room, const QString &server)
{
  foreach (MUC *muc, m_mucs)
  {
    if (muc->room.compare(room) == 0 && muc->server.compare(server) == 0)
    {
      return muc;
    }
  }
  return 0;
}

MUC *MUCControl::getMUC(const QString &roomJid)
{
  return getMUC(roomJid.section('@', 0, 0), roomJid.section("@", 1, 1));
}

User *MUCControl::getUser(const QString &room, const QString &server, const QString &nick)
{
  MUC *muc = getMUC(room, server);
  if (muc)
  {
    foreach (User *user, muc->users)
    {
      if (user->nick.compare(nick) == 0)
        return user;
    }
  }
  return 0;
}

void MUCControl::deleteMUC(MUC *muc)
{
  for (int i = 0; i < m_mucs.count(); i++)
  {
    if (muc == m_mucs[i])
    {
      emit deletedMUC(muc);
      for (int j = 0; j < m_mucs[i]->users.count(); j++)
        delete m_mucs[i]->users.takeAt(j);
      delete m_mucs.takeAt(i);
    }
  }
}

void MUCControl::deleteUser(User *user)
{
  for (int i = 0; i < user->muc->users.count(); i++)
  {
    if (user->muc->users[i] == user)
      delete user->muc->users.takeAt(i);
  }
}

int MUCControl::getUserActions(const QString &roomJid, const QString &nick)
{
  MUC *muc = getMUC(roomJid);
  if (muc)
  {
    foreach (User *user, muc->users)
    {
      if (user->nick.compare(nick) == 0)
      {          
        return getUserActions(muc->me, user);
      }
    }
  }
  return 0;
}

int MUCControl::getUserActions(User *me, User *user)
{
  int result = None;
  switch (me->aff)
  {
    case MUCItem::Owner:
      result += Ban;

      if (user->aff == MUCItem::Owner)
        result += RevokeOwnership;
      else
        result += GrantOwnership;

      if (user->aff == MUCItem::Admin)
        result += RevokeAdmin;
      else
        result += GrantAdmin;

    case MUCItem::Admin:

      if (user->role != MUCItem::Moderator)
        result += GrantModeration;
      else
      {
        if (user->aff != MUCItem::Admin && user->aff != MUCItem::Owner && me->aff > user->aff)
          result += RevokeModeration;
      }

      if (user->aff == MUCItem::NoAffiliation)
        result += GrantMembership;
      else
        result += RevokeMembership;

      if (user->aff < MUCItem::Admin)
        if (!(result & Ban))
          result += Ban;
      break;
    default:
      break;
  }
  if (me->role == MUCItem::Moderator)
  {
    // Kicking
    if (user->role == MUCItem::Visitor || user->role == MUCItem::Participant)
      result += Kick;
    if (user->role == MUCItem::Visitor)
      result += GrantVoice;
    if (user->role == MUCItem::Participant && user->aff < MUCItem::Admin)
      result += RevokeVoice;
  }
  return result;
}

User *MUCControl::getUser(const QString &roomJid, const QString &nick)
{
  MUC *muc = getMUC(roomJid);
  if (muc)
  {
    foreach (User *user, muc->users)
    {
      if (user->nick.compare(nick) == 0)
        return user;
    }
  }
  return 0;
}

void MUCControl::groupChatJoined(const Jid &jid)
{
  MUC *muc = getMUC(jid.bare());
  if (muc)
    emit connectedToMUC(muc);
}

void MUCControl::groupChatLeft(const Jid &jid)
{
  MUC *muc = getMUC(jid.bare());
  if (muc)
  {
    emit disconnectedFromMUC(muc);
    while (!muc->users.empty())
    {
      delete muc->users.takeAt(0);
    }
    muc->connected = false;
    muc->me = 0;
  }
}

void MUCControl::groupChatPresence(const Jid &jid, const Status &status)
{
  User *user = getUser(jid.bare(), jid.resource());
  if (user)
  {
    if (status.type() != Status::Offline)
    {
      user->role = status.mucItem().role();
      user->aff = status.mucItem().affiliation();
      user->status = status.type();
      user->jid = status.mucItem().jid();
      emit setUserStatus(user);
    }
    else
    {
      user->status = Status::Offline;
      emit setUserStatus(user);
      deleteUser(user);
    }
  }
  else
  {
    MUC *muc = getMUC(jid.bare());
    if (muc)
    {
      user = new User;
      user->role = status.mucItem().role();
      user->aff = status.mucItem().affiliation();
      user->status = status.type();
      user->jid = status.mucItem().jid();
      user->nick = jid.resource();
      user->muc = muc;
      if (muc->ownNick.compare(user->nick) == 0)
        muc->me = user;
      muc->users << user;
      emit setUserStatus(user);
    }
  }
}

void MUCControl::groupChatError(const Jid &jid, int, const QString &message)
{
  MUC *muc = getMUC(jid.bare());
  if (muc)
    emit error(muc, message);
}

void MUCControl::groupChatJoin(MUC *muc)
{
  m_makneto->getConnection()->groupChatJoin(muc->server, muc->room, muc->ownNick);
  m_makneto->actionNewSession(muc->jid.bare(), GroupChat, muc->jid.resource());
}

void MUCControl::groupChatLeave(MUC *muc)
{
  m_makneto->getConnection()->groupChatLeave(muc->jid);
}

void MUCControl::loadMUCs()
{
  KConfig config("maknetorc");
  KConfigGroup bookmarks(&config, "MUC_Bookmarks");
  QStringList list = bookmarks.readEntry("bookmarks", QStringList());
  for (int i = 0; i < list.count(); i++)
  {
    newMUC(list[i]);
  }
}

void MUCControl::saveMUCs()
{
  KConfig config("maknetorc");
  KConfigGroup bookmarks(&config, "MUC_Bookmarks");
  QStringList list;
  foreach (MUC *muc, m_mucs)
    list << muc->jid.full();
  bookmarks.writeEntry("bookmarks", list);
}

void MUCControl::_grantVoice(const Jid &jid, const QString &nick, const QString &reason)
{
  emit grantVoice(jid, nick, reason);
}

void MUCControl::_revokeVoice(const Jid &jid, const QString &nick, const QString &reason)
{
  emit revokeVoice(jid, nick, reason);
}

void MUCControl::_kickUser(const Jid &jid, const QString &nick, const QString &reason)
{
  emit kickUser(jid, nick, reason);
}

void MUCControl::_banUser(const Jid &jid, const Jid &userJid, const QString &reason)
{
  emit banUser(jid, userJid, reason);
}

void MUCControl::_grantMembership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  emit grantMembership(jid, userJid, reason);
}

void MUCControl::_revokeMembership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  emit revokeMembership(jid, userJid, reason);
}

void MUCControl::_grantModeration(const Jid &jid, const QString &nick, const QString &reason)
{
  emit grantModeration(jid, nick, reason);
}

void MUCControl::_revokeModeration(const Jid &jid, const QString &nick, const QString &reason)
{
  emit revokeModeration(jid, nick, reason);
}

void MUCControl::_grantAdministration(const Jid &jid, const Jid &userJid, const QString &reason)
{
  emit grantAdministration(jid, userJid, reason);
}

void MUCControl::_revokeAdministration(const Jid &jid, const Jid &userJid, const QString &reason)
{
  emit revokeAdministration(jid, userJid, reason);
}

void MUCControl::_grantOwnership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  emit grantOwnership(jid, userJid, reason);
}

void MUCControl::_revokeOwnership(const Jid &jid, const Jid &userJid, const QString &reason)
{
  emit revokeOwnership(jid, userJid, reason);
}
