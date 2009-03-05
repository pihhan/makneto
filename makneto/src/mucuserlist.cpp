/*
 * mucuserlist.h
 *
 * Implementation of user list widget
 *
 * Copyright (C) 2009 Radek Novacek <rad.n@centrum.cz>
 */

#include "mucuserlist.h"

#include "muccontrol.h"
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

QString StatusStr[] = { "Offline", "Online", "Away", "XA", "DND", "Invisible", "FFC" };

User::User(const QString &_nick, const Jid &_jid, const MUCItem::Affiliation &_affiliation, const MUCItem::Role &_role, const Status::Type &_status, QListWidget *listWidget)
{
  nick = _nick;
  jid = _jid;
  affiliation = _affiliation;
  role = _role;
  status = _status;
  item = new QListWidgetItem(nick, listWidget);
  listWidget->addItem(item);
  item->setData(Qt::UserRole, qVariantFromValue(this));
}

void User::update()
{
  QString text = QString("<%1> %2").arg(StatusStr[status], nick);
  switch (role)
  {
    case MUCItem::Moderator:
      text += " [Moderator]";
      break;
    case MUCItem::Visitor:
      text += " [Visitor]";
      break;
    default:
      break;
  }
  switch (affiliation)
  {
    case MUCItem::Owner:
      text += " {Owner}";
      break;
    case MUCItem::Admin:
      text += " {Admin}";
      break;
    default:
      break;
  }
  
  item->setText(text);
  if (role == MUCItem::Visitor)
  {
    KIcon icon("maknetobubblegray");
    item->setIcon(icon);
  }
  else
  {
    KIcon icon("maknetobubble");
    item->setIcon(icon);
  }
}

MUCUserList::MUCUserList(MUCControl *parent) : QListWidget(parent)
{
  m_mucControl = parent;
  
  setContextMenuPolicy(Qt::DefaultContextMenu);
  
  // Create popup menu
  popupMenu = new QMenu(this);
  
  grantVoice           = popupMenu->addAction(tr("Grant voice"),           m_mucControl, SLOT(_grantVoice()));
  revokeVoice          = popupMenu->addAction(tr("Revoke voice"),          m_mucControl, SLOT(_revokeVoice()));
  kickUser             = popupMenu->addAction(tr("Kick"),                  m_mucControl, SLOT(_kickUser()));
  banUser              = popupMenu->addAction(tr("Ban"),                   m_mucControl, SLOT(_banUser()));
  QMenu *roleMenu = popupMenu->addMenu(tr("Change role"));
  grantModeration      = roleMenu->addAction(tr("Grant moderation"),      m_mucControl, SLOT(_grantModeration()));
  revokeModeration     = roleMenu->addAction(tr("Revoke moderation"),     m_mucControl, SLOT(_revokeModeration()));
  QMenu *affMenu = popupMenu->addMenu(tr("Change affiliation"));
  grantOwnership       = affMenu->addAction(tr("Grant ownership"),       m_mucControl, SLOT(_grantOwnership()));
  revokeOwnership      = affMenu->addAction(tr("Revoke ownership"),      m_mucControl, SLOT(_revokeOwnership()));
  grantAdministration  = affMenu->addAction(tr("Grant administration"),  m_mucControl, SLOT(_grantAdministration()));
  revokeAdministration = affMenu->addAction(tr("Revoke administration"), m_mucControl, SLOT(_revokeAdministration()));
  grantMembership      = affMenu->addAction(tr("Grant membership"),      m_mucControl, SLOT(_grantMembership()));
  revokeMembership     = affMenu->addAction(tr("Revoke membership"),     m_mucControl, SLOT(_revokeMembership()));
  
}
  
User *MUCUserList::findUser(const QString &nick)
{ 
  for (int i = 0; i < users.size(); ++i)
  {
    if (users[i]->nick.compare(nick) == 0)
    {
      return users[i];
    }
  }
  return 0;
}

void MUCUserList::contextMenuEvent(QContextMenuEvent *event)
{
  QListWidgetItem *item = itemAt(event->pos());
  if (!item) return;
  User *user = qVariantValue<User *> (item->data(Qt::UserRole));
  
  m_affectedUser = user->nick;
  m_affectedUserJid = user->jid;
  
  grantVoice->setVisible(false);
  revokeVoice->setVisible(false);
  
  kickUser->setVisible(false);
  banUser->setVisible(false);
  
  grantOwnership->setVisible(false);
  revokeOwnership->setVisible(false);
  
  revokeAdministration->setVisible(false);
  grantAdministration->setVisible(false);
  
  grantMembership->setVisible(false);
  revokeMembership->setVisible(false);
  
  grantModeration->setVisible(false);
  revokeModeration->setVisible(false);
  
  switch (m_mucControl->affiliation())
  {
    case MUCItem::Owner:
      // Banning
      banUser->setVisible(true);
      // Ownership
      if (user->affiliation == MUCItem::Owner)
        revokeOwnership->setVisible(true);
      else
        grantOwnership->setVisible(true);
      // Administration
      if (user->affiliation == MUCItem::Admin)
        revokeAdministration->setVisible(true);
      if (user->affiliation < MUCItem::Admin)
        grantAdministration->setVisible(true);
    case MUCItem::Admin:
      // Moderation
      if (user->role != MUCItem::Moderator)
      {
        grantModeration->setVisible(true);
      }
      else
      {
        if (user->affiliation != MUCItem::Admin && user->affiliation != MUCItem::Owner && m_mucControl->affiliation() > user->affiliation)
        {
          revokeModeration->setVisible(true);
        }
      }
      // Membership
      if (user->affiliation == MUCItem::NoAffiliation)
      {
        grantMembership->setVisible(true);
      }
      if (user->affiliation == MUCItem::Member)
      {
        revokeMembership->setVisible(true);
      }
      // Banning
      if (user->affiliation < MUCItem::Admin)
        banUser->setVisible(true);
      break;
    default:
      break;
  }
  switch (m_mucControl->role())
  {
    case MUCItem::Moderator:
      // Kicking
      if (user->role == MUCItem::Visitor || user->role == MUCItem::Participant)
        kickUser->setVisible(true);
      if (user->role == MUCItem::Visitor)
        grantVoice->setVisible(true);
      if (user->role == MUCItem::Participant && user->affiliation < MUCItem::Admin)
        revokeVoice->setVisible(true);
      break;
    default:
      break;

  }
  popupMenu->popup(event->globalPos());
}

void MUCUserList::updateUser(const QString &nick, const Jid &jid, const MUCItem::Affiliation &affiliation, const MUCItem::Role &role, const Status::Type &status)
{
  User *user = findUser(nick);
  if (user)
  {
    if (!jid.isNull())
      user->jid = jid;
    if (affiliation != MUCItem::UnknownAffiliation)
      user->affiliation = affiliation;
    if (role != MUCItem::UnknownRole)
      user->role = role;
    if (status != Status::Online)
      user->status = status;
    user->update();
  }
  else
  {
    user = new User(nick, jid, affiliation, role, status, this);
    users << user;
    user->update();
  }
}

bool MUCUserList::renameUser(const QString &nick, const QString &newNick)
{
  User *user = findUser(nick);
  if (user)
  {
    user->nick = newNick;
    return true;
  }
  return false;
}
