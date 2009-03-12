/*
 * mucview.cpp
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#include "mucview.h"

#include "sessiontabmanager.h"
#include "sessionview.h"
#include "muccontrol.h"

#include <iostream>
#include <ctime>
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include <KPushButton>

MUCView::MUCView(QWidget */*parent*/, Makneto *makneto) : m_makneto(makneto)
{
  m_mainlayout = new QVBoxLayout(this);
  m_buttonslayout = new QHBoxLayout();

  // buttons
  m_createMUC = new QPushButton(KIconLoader::global()->loadIcon("list-add-user", KIconLoader::Toolbar, KIconLoader:: SizeSmall), i18n("&Create MUC"), this);
  m_buttonslayout->addWidget(m_createMUC);
  connect(m_createMUC, SIGNAL(clicked(bool)), SLOT(createMUCClicked(bool)));

  m_joinMUC = new QPushButton(KIconLoader::global()->loadIcon("edit-find-user", KIconLoader::Toolbar, KIconLoader:: SizeSmall), i18n("&Join MUC"), this);
  m_buttonslayout->addWidget(m_joinMUC);
  connect(m_joinMUC, SIGNAL(clicked(bool)), SLOT(joinMUCClicked(bool)));

  m_MUCbookmarks = new QListWidget(this);
  connect(m_MUCbookmarks, SIGNAL(itemDoubleClicked(QListWidgetItem *)), SLOT(bookmarkedMUC(QListWidgetItem *)));
  // finally add to layout
  m_mainlayout->addLayout(m_buttonslayout);
  m_mainlayout->addWidget(m_MUCbookmarks);

  setLayout(m_mainlayout);
  
  connect(m_makneto->getConnection(), SIGNAL(groupChatJoined(const Jid &)), 
          SLOT(groupChatJoined(const Jid &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatLeft(const Jid &)), 
          SLOT(groupChatLeft(const Jid &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatPresence(const Jid &, const Status &)), 
          SLOT(groupChatPresence(const Jid &, const Status &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatError(const Jid &, int, const QString &)), 
          SLOT(groupChatError(const Jid &, int, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatVoiceGranted(const Jid &, const QString &, const QString &)),
          SLOT(voiceGranted(const Jid &, const QString &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatVoiceRevoked(const Jid &, const QString &, const QString &)),
          SLOT(voiceRevoked(const Jid &, const QString &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatVoiceRequested(const Jid &, const QString &)),
          SLOT(voiceRequested(const Jid &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatSubjectChanged(const Jid &, const QString &)),
          SLOT(subjectChanged(const Jid &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatUserKicked(const Jid &, const QString &, const QString &, const QString &)),
          SLOT(userKicked(const Jid &, const QString &, const QString &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatUserBanned(const Jid &, const QString &, const QString &, const QString &)),
          SLOT(userBanned(const Jid &, const QString &, const QString &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatUserRemovedAsNonMember(const Jid &, const QString &)),
          SLOT(userRemovedAsNonMember(const Jid &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatMembershipGranted(const Jid &, const Jid &, const QString &)),
          SLOT(membershipGranted(const Jid &, const Jid &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatMembershipRevoked(const Jid &, const Jid &, const QString &)),
          SLOT(membershipRevoked(const Jid &, const Jid &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatModerationGranted(const Jid &, const QString &, const QString &)),
          SLOT(moderationGranted(const Jid &, const QString &, const QString &)));
  connect(m_makneto->getConnection(), SIGNAL(groupChatModerationRevoked(const Jid &, const QString &, const QString &)),
          SLOT(moderationRevoked(const Jid &, const QString &, const QString &)));

  loadBookmarks();
}

MUCView::~MUCView()
{
  
}

void MUCView::loadBookmarks(void)
{
  KConfig config("maknetorc");
  KConfigGroup bookmarks(&config, "MUC_Bookmarks");
  QStringList list = bookmarks.readEntry("bookmarks", QStringList());
  tMUC *muc;
  QListWidgetItem *item;
  for (int i = 0; i < list.count(); i++)
  {
    muc = new tMUC;
    muc->room = list[i].section('@', 0, 0);
    muc->server = list[i].section(QRegExp("[@/]"), 1, 1);
    muc->nick = list[i].section('/', 1, 1);
    item = new QListWidgetItem(list[i]);
    item->setData(Qt::UserRole, qVariantFromValue<tMUC *>(muc));
    m_MUCbookmarks->addItem(item);
  }
}

void MUCView::saveBookmarks(void)
{
  KConfig config("maknetorc");
  KConfigGroup bookmarks(&config, "MUC_Bookmarks");
  QStringList list;
  tMUC *muc;
  for (int i = 0; i < m_MUCbookmarks->count(); i++)
  {
    muc = qVariantValue<tMUC *>(m_MUCbookmarks->item(i)->data(Qt::UserRole));
    list << muc->room + '@' + muc->server + '/' + muc->nick;
  }
  bookmarks.writeEntry("bookmarks", list);
}

SessionView *MUCView::getSessionByJid(const Jid &jid)
{
  return m_makneto->getMaknetoMainWindow()->getMaknetoView()->getSessionTabManager()->findSession(jid.bare());
}

void MUCView::createMUCClicked(bool /*toggled*/)
{
  std::cout << "Create new MUC" << std::endl;
}

void MUCView::joinMUCClicked(bool /*toggled*/)
{
  std::cout << "Join MUC" << std::endl;
  QString nick, server, room;
  if (getConferenceSetting(room, server, nick))
  {
    // Join to group chat
    m_makneto->getConnection()->groupChatJoin(server, room, nick);
    m_makneto->actionNewSession(room + '@' + server, GroupChat, nick);
    
    // Create bookmark and add it to list
    tMUC *muc = new tMUC;
    muc->room = room;
    muc->server = server;
    muc->nick = nick;
    QListWidgetItem *item = new QListWidgetItem(room + '@' + server + '/' + nick);
    item->setData(Qt::UserRole, qVariantFromValue<tMUC *>(muc));
    m_MUCbookmarks->addItem(item);
    saveBookmarks();
  }
}

void MUCView::bookmarkedMUC(QListWidgetItem *item)
{
  if (!m_makneto->getConnection()->isOnline())
  {
    QMessageBox::critical(this, "Makneto", tr("Connect to the Jabber server before opening MUC!"));
    return;
  }
  tMUC *muc = qVariantValue<tMUC *>(item->data(Qt::UserRole));
  qDebug() << "Bookmarked MUC: " << muc->room + '@' + muc->server + '/' + muc->nick;
  
  SessionView *session = getSessionByJid(Jid(muc->room + '@' + muc->server));
  if (!session)
  {
    m_makneto->getConnection()->groupChatJoin(muc->server, muc->room, muc->nick, QString(), -1, -1, time(NULL));
    m_makneto->actionNewSession(muc->room + '@' + muc->server, GroupChat, muc->nick);
  }
  else
  {
    m_makneto->getMaknetoMainWindow()->getMaknetoView()->getSessionTabManager()->bringToFront(session);
  }
}

void MUCView::groupChatJoined(const Jid &jid)
{
  SessionView *session = getSessionByJid(jid.bare());
  if (session)
  {
    emit session->getMUCControl()->setConnected(true);
  }
  // TODO: Transform to better form (maybe status bar ?)
  QMessageBox::information(this, "Connected", "You have been connected to \"" + jid.bare() + "\".");
}

void MUCView::groupChatLeft(const Jid &jid)
{
  SessionView *session = getSessionByJid(jid.bare());
  if (session)
  {
    emit session->getMUCControl()->setConnected(false);
  }
  QMessageBox::information(this, "Disconnected", "You have been disconnected from \"" + jid.bare() + "\".");
}

void MUCView::groupChatPresence(const Jid &jid, const Status &status)
{
  SessionView *session = getSessionByJid(jid.bare());
  if (session)
  {
    emit session->getMUCControl()->setPresence(jid, status);
  }
  qDebug() << "MUC Presence: " << jid.full() << "; Status: " << status.typeString();
}

void MUCView::groupChatError(const Jid &, int, const QString &error)
{
  QMessageBox::critical(this, "Error", error);
}

bool MUCView::getConferenceSetting(QString &room, QString &server, QString &nick)
{
  KDialog *dialog = new KDialog(this);
  dialog->setCaption("Join chat room");
  dialog->setModal(true);
  dialog->setSizeGripEnabled(true);
  QWidget *w = new QWidget(dialog);
  QGridLayout *layout = new QGridLayout(w);
  
  QLabel *lRoom = new QLabel(tr("Room:"), w);
  QLineEdit *eRoom = new QLineEdit(w);
  lRoom->setBuddy(eRoom);
  
  QLabel *lServer = new QLabel(tr("Server:"), w);
  QLineEdit *eServer = new QLineEdit(w);
  lRoom->setBuddy(eServer);
  
  QLabel *lNick = new QLabel(tr("Nickname:"), w);
  QLineEdit *eNick = new QLineEdit();
  lRoom->setBuddy(eNick);

  layout->addWidget(lRoom, 0, 0);
  layout->addWidget(eRoom, 0, 1);
  layout->addWidget(lServer, 1, 0);
  layout->addWidget(eServer, 1, 1);
  layout->addWidget(lNick, 2, 0);
  layout->addWidget(eNick, 2, 1);
  dialog->setMainWidget(w);
  dialog->setButtons(KDialog::Ok | KDialog::Cancel);

  
  bool result = false;
  if (dialog->exec() == QDialog::Accepted)
  {
    nick = eNick->text();
    room = eRoom->text();
    server = eServer->text();
    result = true;
  }
  delete dialog;
  return result;
}

void MUCView::voiceGranted(const Jid &jid, const QString &nick, const QString &reason)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->voiceGranted(nick, reason);
}

void MUCView::voiceRevoked(const Jid &jid, const QString &nick, const QString &reason)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->voiceRevoked(nick, reason);
}

void MUCView::voiceRequested(const Jid &jid, const QString &nick)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->voiceRequested(nick);
}

void MUCView::subjectChanged(const Jid &jid, const QString &subject)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->subjectChanged(subject);
}

void MUCView::userKicked(const Jid &jid, const QString &nick, const QString &reason, const QString &actor)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->userKicked(nick, reason, actor);
}

void MUCView::userBanned(const Jid &jid, const QString &nick, const QString &reason, const QString &actor)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->userBanned(nick, reason, actor);
}

void MUCView::userRemovedAsNonMember(const Jid &jid, const QString &nick)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->userRemovedAsNonMember(nick);
}

void MUCView::membershipGranted(const Jid &jid, const Jid &userJid, const QString &reason)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->membershipGranted(userJid, reason);
}

void MUCView::membershipRevoked(const Jid &jid, const Jid &userJid, const QString &reason)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->membershipRevoked(userJid, reason);
}

void MUCView::moderationGranted(const Jid &jid, const QString &nick, const QString &reason)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->moderationGranted(nick, reason);
}

void MUCView::moderationRevoked(const Jid &jid, const QString &nick, const QString &reason)
{
  SessionView *session = getSessionByJid(jid);
  if (session)
    emit session->getMUCControl()->moderationRevoked(nick, reason);
}
