/*
 * mucview.cpp
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#include "mucview.h"

#include <iostream>
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include <KPushButton>

MUCView::MUCView(QWidget *parent, Makneto *makneto) : m_makneto(makneto)
{
  m_mainlayout = new QVBoxLayout(this);
  m_buttonslayout = new QHBoxLayout(this);

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
  QListWidgetItem *item;
  for (int i = 0; i < m_MUCbookmarks->count(); i++)
  {
    muc = qVariantValue<tMUC *>(m_MUCbookmarks->item(i)->data(Qt::UserRole));
    list << muc->room + '@' + muc->server + '/' + muc->nick;
  }
  bookmarks.writeEntry("bookmarks", list);
}

void MUCView::createMUCClicked(bool toggled)
{
  std::cout << "Create new MUC" << std::endl;
}

void MUCView::joinMUCClicked(bool toggled)
{
  std::cout << "Join MUC" << std::endl;
  bool ok;
  QString nick, server, room;
  if (getConferenceSetting(room, server, nick))
  {
    // Join to group chat
    m_makneto->getConnection()->groupChatJoin(server, room, nick);
    
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
  tMUC *muc = qVariantValue<tMUC *>(item->data(Qt::UserRole));
  qDebug() << "Bookmarked MUC: " << muc->room + '@' + muc->server + '/' + muc->nick;
  m_makneto->getConnection()->groupChatJoin(muc->server, muc->room, muc->nick);
  m_makneto->actionNewSession(muc->room + '@' + muc->server, GroupChat);
}

void MUCView::groupChatJoined(const Jid &jid)
{
  // TODO: Transform to better form (maybe status bar ?)
  QMessageBox::information(this, "Connected", "You have been connected to \"" + jid.bare() + "\".");
}

void MUCView::groupChatLeft(const Jid &jid)
{
  QMessageBox::information(this, "Disconnected", "You have been disconnected from \"" + jid.bare() + "\".");
}

void MUCView::groupChatPresence(const Jid &, const Status &)
{
  
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
  QGridLayout *layout = new QGridLayout(dialog);
  
  QLabel *lRoom = new QLabel(dialog, tr("&Room:"));
  QLineEdit *eRoom = new QLineEdit(dialog);
  lRoom->setBuddy(eRoom);
  
  QLabel *lServer = new QLabel(dialog, tr("&Server:"));
  QLineEdit *eServer = new QLineEdit(dialog);
  lRoom->setBuddy(eServer);
  
  QLabel *lNick = new QLabel(dialog, tr("&Nickname:"));
  QLineEdit *eNick = new QLineEdit(dialog);
  lRoom->setBuddy(eNick);

  layout->addWidget(lRoom, 0, 0);
  layout->addWidget(eRoom, 0, 1);
  layout->addWidget(lServer, 1, 0);
  layout->addWidget(eServer, 1, 1);
  layout->addWidget(lNick, 2, 0);
  layout->addWidget(eNick, 2, 1);
  QHBoxLayout *buttonLayout = new QHBoxLayout(dialog);
  buttonLayout->addWidget(dialog->button(KDialog::Ok));
  buttonLayout->addWidget(dialog->button(KDialog::Cancel));
  layout->addLayout(buttonLayout, 3, 0, 1, 2, Qt::AlignRight);
  dialog->setLayout(layout);
  
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
