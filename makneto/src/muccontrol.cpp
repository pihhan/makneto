/*
 * muccontrol.cpp
 *
 * GUI for managing MUCs
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#include "muccontrol.h"
#include "settings.h"
#include "mucuserlist.h"
#include "sessionview.h"

#include <QListWidget>
#include <QInputDialog>
#include <QToolButton>
#include <QMessageBox>

QString AffiliationStr[] = { "Unknown", "Outcast", "None", "Member", "Admin", "Owner" };
QString RoleStr[] = { "UnknownRole", "NoRole", "Visitor", "Participant", "Moderator" };

MUCControl::MUCControl(SessionView *parent, Makneto *makneto, const QString &jid, const QString &nick)
  : QWidget(parent), m_sessionView(parent), m_makneto(makneto), m_jid(Jid(jid)), m_nick(nick)
{
  m_makneto = makneto;
  m_jid = Jid(jid);
  
  QVBoxLayout *layout = new QVBoxLayout(this);
  QHBoxLayout *toolLayout = new QHBoxLayout(this);
  
  m_userlist = new MUCUserList(this);
  
  m_roleLabel = new QLabel(this);
  m_roleLabel->setText(QString("<b>Role: </b>%1\t<b>Affiliation: </b>%2").arg("None", "None"));
  toolLayout->addWidget(m_roleLabel);
  QToolButton *btnRequestVoice = new QToolButton(this);
  btnRequestVoice->setIcon(KIcon("maknetobubble"));
  btnRequestVoice->setToolTip(tr("Request voice"));
  toolLayout->addWidget(btnRequestVoice);
  
  layout->addLayout(toolLayout);
  layout->addWidget(m_userlist);
  setLayout(layout);
  
  connect(btnRequestVoice, SIGNAL(clicked(bool)), this, SLOT(_requestVoice()));
  
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

void MUCControl::doDisconnect(const Jid &jid)
{
  emit groupChatDisconnect(jid);
}

void MUCControl::setConnected(bool connected)
{
  if (connected)
  {
    m_sessionView->infoMessage(tr("You have entered this room"));
  }
  else
  {
    m_sessionView->infoMessage(tr("You have left this room"));
  }
  m_connected = connected;
  m_sessionView->setEnabled(connected);
}

void MUCControl::setPresence(const Jid &jid, const Status &status)
{
  if (jid.resource() == m_nick)
  {
    m_roleLabel->setText(QString("<b>Affiliation: </b>%1\t<b>Role: </b>%2").arg(AffiliationStr[status.mucItem().affiliation()], RoleStr[status.mucItem().role()]));
    m_role = status.mucItem().role();
    m_affiliation = status.mucItem().affiliation();
  }
  m_userlist->updateUser(jid.resource(), status.mucItem().jid(), status.mucItem().affiliation(), status.mucItem().role(), status.type());
}

void MUCControl::voiceGranted(const QString &nick, const QString &reason)
{
  
}

void MUCControl::voiceRevoked(const QString &nick, const QString &reason)
{
  
}

void MUCControl::voiceRequested(const QString &nick)
{
  
}

void MUCControl::subjectChanged(const QString &subject)
{
  m_sessionView->infoMessage("Subject: " + subject);
}

void MUCControl::userKicked(const QString &nick, const QString &reason, const QString &actor)
{
  QString message = QString(tr("been kicked from this room"));
  if (!actor.isEmpty())
    message += QString(tr(" by %1")).arg(actor);
  if (!reason.isEmpty())
    message += QString(tr(" (Reason: %1)")).arg(reason);
  if (nick == m_nick)
  {
    message = tr("You have ") + message;
    QMessageBox::critical(this, "Makneto", message);
    setConnected(false);
  }
  else
    message = QString(tr("%1 has ")).arg(nick) + message;
}

void MUCControl::userBanned(const QString &nick, const QString &reason, const QString &actor)
{
  QString message = QString("User \"%1\" banned").arg(nick);
  if (!reason.isEmpty())
    message += QString(" (%2)").arg(reason);
  if (!actor.isEmpty())
    message += QString(" by %3").arg(actor);
  m_sessionView->infoMessage(message);
}

void MUCControl::userRemovedAsNonMember(const QString &nick)
{
  
}

void MUCControl::membershipGranted(const Jid &userJid, const QString &reason)
{
  
}

void MUCControl::membershipRevoked(const Jid &userJid, const QString &reason)
{
  
}

void MUCControl::moderationGranted(const QString &nick, const QString &reason)
{
  
}

void MUCControl::moderationRevoked(const QString &nick, const QString &reason)
{
  
}

void MUCControl::_grantVoice()
{
  emit grantVoice(m_jid, m_userlist->affectedUser());
}

void MUCControl::_revokeVoice()
{
  bool ok;
  QString reason = QInputDialog::getText(this, "Makneto", tr("Reason for revoking voice"), QLineEdit::Normal, QString(), &ok);
  if (ok)
    emit revokeVoice(m_jid, m_userlist->affectedUser(), reason);
}

void MUCControl::_requestVoice()
{
  emit requestVoice(m_jid);
}

void MUCControl::_subjectChange()
{
  
}

void MUCControl::_kickUser()
{
  bool ok;
  QString reason = QInputDialog::getText(this, "Makneto", tr("Reason for kicking"), QLineEdit::Normal, QString(), &ok);
  if (ok)
    emit kickUser(m_jid, m_userlist->affectedUser(), reason);
}

void MUCControl::_banUser()
{
  bool ok;
  QString reason = QInputDialog::getText(this, "Makneto", tr("Reason for banning user"), QLineEdit::Normal, QString(), &ok);
  if (ok)
    emit banUser(m_jid, m_userlist->affectedUserJid(), reason);
}

void MUCControl::_grantMembership()
{
  emit grantMembership(m_jid, m_userlist->affectedUserJid());
}

void MUCControl::_revokeMembership()
{
  bool ok;
  QString reason = QInputDialog::getText(this, "Makneto", tr("Reason for revoking membership"), QLineEdit::Normal, QString(), &ok);
  if (ok)
    emit revokeMembership(m_jid, m_userlist->affectedUserJid(), reason);
}

void MUCControl::_grantModeration()
{
  emit grantModeration(m_jid, m_userlist->affectedUser());
}

void MUCControl::_revokeModeration()
{
  bool ok;
  QString reason = QInputDialog::getText(this, "Makneto", tr("Reason for revoking moderation"), QLineEdit::Normal, QString(), &ok);
  if (ok)
    emit revokeModeration(m_jid, m_userlist->affectedUser(), reason);
}

void MUCControl::_grantAdministration()
{
  emit grantAdministration(m_jid, m_userlist->affectedUserJid());
}

void MUCControl::_revokeAdministration()
{
  bool ok;
  QString reason = QInputDialog::getText(this, "Makneto", tr("Reason for revoking moderation"), QLineEdit::Normal, QString(), &ok);
  if (ok)
    emit revokeAdministration(m_jid, m_userlist->affectedUserJid(), reason);
}

void MUCControl::_grantOwnership()
{
  emit grantOwnership(m_jid, m_userlist->affectedUserJid());
}

void MUCControl::_revokeOwnership()
{
  bool ok;
  QString reason = QInputDialog::getText(this, "Makneto", tr("Reason for revoking moderation"), QLineEdit::Normal, QString(), &ok);
  if (ok)
    emit revokeOwnership(m_jid, m_userlist->affectedUserJid(), reason);
}

