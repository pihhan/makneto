
#include "mucview.h"

#include "muccontrol.h"
#include "maknetoview.h"
#include "sessiontabmanager.h"
#include "sessionview.h"
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <KStatusBar>
#include <KIconEffect>
#include <QtGui/QColor>
#include <QtGui/QMenu>
#include <QtGui/QAction>

MUCView::MUCView(MaknetoView *view, Makneto *makneto) : QWidget(), m_makneto(makneto), m_maknetoview(view)
{
  m_control = new MUCControl(makneto);
  QVBoxLayout *layout = new QVBoxLayout();
  QPushButton *connectButton = new QPushButton(KIcon("list-add-user"), "Join or create MUC", this);
  layout->addWidget(connectButton);
  connect(connectButton, SIGNAL(pressed()), this, SLOT(joinMUC()));
  tree = new QTreeWidget(this);
  connect(tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
  layout->addWidget(tree);
  setLayout(layout);
  tree->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(tree, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(contextMenu(const QPoint &)));

  connect(m_control, SIGNAL(addMUC(MUC *)), SLOT(addMUC(MUC *)));
  connect(m_control, SIGNAL(connectedToMUC(MUC *)), SLOT(connectedToMUC(MUC *)));
  connect(m_control, SIGNAL(disconnectedFromMUC(MUC *)), SLOT(disconnectedFromMUC(MUC *)));
  connect(m_control, SIGNAL(error(MUC *, const QString &)), SLOT(error(MUC *, const QString &)));
  connect(m_control, SIGNAL(setUserStatus(User *)), SLOT(setUserStatus(User *)));
  connect(m_control, SIGNAL(deletedMUC(MUC *)), SLOT(deletedMUC(MUC *)));

  connect(this, SIGNAL(connectToMUC(MUC *)), m_control, SLOT(groupChatJoin(MUC *)));
  connect(this, SIGNAL(disconnectFromMUC(MUC *)), m_control, SLOT(groupChatLeave(MUC *)));
  connect(this, SIGNAL(deleteMUC(MUC *)), m_control, SLOT(deleteMUC(MUC *)));


  m_control->loadMUCs();

  createIcons();
}

void MUCView::createIcons()
{
  iconOnline[0] = KIcon("maknetoonline");
  iconOnline[1] = getColorizedIcon(iconOnline[0], Qt::blue);
  iconOnline[2] = getColorizedIcon(iconOnline[0], Qt::gray);
  iconAway[0] = KIcon("maknetoaway");
  iconAway[1] = getColorizedIcon(iconAway[0], Qt::blue);
  iconAway[2] = getColorizedIcon(iconAway[0], Qt::gray);
  iconXA[0] = KIcon("maknetoxa");
  iconXA[1] = getColorizedIcon(iconXA[0], Qt::blue);
  iconXA[2] = getColorizedIcon(iconXA[0], Qt::gray);
  iconDND[0] = KIcon("maknetodnd");
  iconDND[1] = getColorizedIcon(iconDND[0], Qt::blue);
  iconDND[2] = getColorizedIcon(iconDND[0], Qt::gray);
  iconInvisible[0] = KIcon("maknetoinvisible");
  iconInvisible[1] = getColorizedIcon(iconInvisible[0], Qt::blue);
  iconInvisible[2] = getColorizedIcon(iconInvisible[0], Qt::gray);
  iconFFC[0] = KIcon("maknetoffc");
  iconFFC[1] = getColorizedIcon(iconFFC[0], Qt::blue);
  iconFFC[2] = getColorizedIcon(iconFFC[0], Qt::gray);
  iconOffline[0] = KIcon("maknetooffline");
  iconOffline[1] = getColorizedIcon(iconOffline[0], Qt::blue);
  iconOffline[2] = getColorizedIcon(iconOffline[0], Qt::gray);
}

KIcon MUCView::getColorizedIcon(const KIcon &icon, QColor color)
{
  QImage img = icon.pixmap(32);
  KIconEffect::colorize(img, color, 1.0);
  return KIcon(QIcon(QPixmap::fromImage(img)));
}

void MUCView::contextMenu(const QPoint &point)
{
  QTreeWidgetItem *item = tree->itemAt(point);
  if (item)
  {
    if (item->parent())
    {
      // User
      MUC *muc = qVariantValue<MUC *> (item->parent()->data(0, Qt::UserRole));
      User *user = qVariantValue<User *> (item->data(0, Qt::UserRole));
      if (muc && user)
      {
        int actions = m_control->getUserActions(muc->me, user);
        QMenu *menu = new QMenu(0);
        if (actions & GrantOwnership)
          menu->addAction(tr("Grant &Ownership"))->setData(GrantOwnership);
        if (actions & RevokeOwnership)
          menu->addAction(tr("Revoke &Ownership"))->setData(GrantOwnership);
        if (actions & GrantAdmin)
          menu->addAction(tr("Grant &Administration"))->setData(GrantAdmin);
        if (actions & RevokeAdmin)
          menu->addAction(tr("Revoke &Administration"))->setData(RevokeAdmin);
        if (actions & GrantModeration)
          menu->addAction(tr("Grant &Moderation"))->setData(GrantModeration);
        if (actions & RevokeModeration)
          menu->addAction(tr("Revoke &Moderation"))->setData(RevokeModeration);
        if (actions & GrantMembership)
          menu->addAction(tr("Grant M&embership"))->setData(GrantMembership);
        if (actions & RevokeMembership)
          menu->addAction(tr("Revoke M&embership"))->setData(RevokeMembership);
        if (actions & Ban)
          menu->addAction(tr("&Ban"))->setData(Ban);
        if (actions & Kick)
          menu->addAction(tr("&Kick"))->setData(Kick);
        if (actions & GrantVoice)
          menu->addAction(tr("Grant &Voice"))->setData(GrantVoice);
        if (actions & RevokeVoice)
          menu->addAction(tr("Revoke &Voice"))->setData(RevokeVoice);
        QAction *result = menu->exec(tree->mapToGlobal(point));
        if (result)
        {
          bool ok;
          switch (result->data().toInt(&ok))
          {
            case GrantOwnership:
              emit m_control->_grantOwnership(muc->jid, user->jid, QString());
              break;
            case RevokeOwnership:
              emit m_control->_revokeOwnership(muc->jid, user->jid, QString());
              break;
            case GrantAdmin:
              emit m_control->_grantAdministration(muc->jid, user->jid, QString());
              break;
            case RevokeAdmin:
              emit m_control->_revokeAdministration(muc->jid, user->jid, QString());
              break;
            case GrantModeration:
              emit m_control->_grantModeration(muc->jid, user->nick, QString());
              break;
            case RevokeModeration:
              emit m_control->_revokeModeration(muc->jid, user->nick, QString());
              break;
            case GrantMembership:
              emit m_control->_grantMembership(muc->jid, user->jid, QString());
              break;
            case RevokeMembership:
              emit m_control->_revokeMembership(muc->jid, user->jid, QString());
              break;
            case Ban:
              emit m_control->_banUser(muc->jid, user->jid, QString());
              break;
            case Kick:
              emit m_control->_kickUser(muc->jid, user->nick, QString());
              break;
            case GrantVoice:
              emit m_control->_grantVoice(muc->jid, user->nick, QString());
              break;
            case RevokeVoice:
              emit m_control->_revokeVoice(muc->jid, user->nick, QString());
              break;
          }
        }
      }
    }
    else
    {
      // MUC
      QMenu *menu = new QMenu(0);
      if (!isMUCConnected(item->text(0)))
      {
        menu->addAction(tr("&Connect to MUC"))->setData(1);
        menu->addAction(tr("&Delete from bookmarks"))->setData(3);
      }
      else
      {
        menu->addAction(tr("&Disconnect from MUC"))->setData(2);
      }
      QAction *result = menu->exec(tree->mapToGlobal(point));
      if (result)
      {
        bool ok;
        MUC *muc;
        switch (result->data().toInt(&ok))
        {
          case 1:
            // Connect to the MUC
            muc = qVariantValue<MUC *>(item->data(0, Qt::UserRole));
            if (muc)
              emit connectToMUC(muc);
            break;
          case 2:
            // Disconnect from the MUC
            muc = qVariantValue<MUC *>(item->data(0, Qt::UserRole));
            if (muc)
              emit disconnectFromMUC(muc);
            break;
          case 3:
            // Delete
            muc = qVariantValue<MUC *>(item->data(0, Qt::UserRole));
            if (muc)
              emit deleteMUC(muc);
        }
      }
    }
  }
}

void MUCView::addMUC(MUC *muc)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(muc->jid.full()), 0);
  item->setData(0, Qt::UserRole, qVariantFromValue<MUC *>(muc));
  tree->addTopLevelItem(item);
}

QTreeWidgetItem *MUCView::getMUCItem(MUC *muc)
{
  foreach (QTreeWidgetItem *item, tree->findItems(muc->jid.bare(), Qt::MatchStartsWith, 0))
  {
    MUC *m = qVariantValue<MUC *> (item->data(0, Qt::UserRole));
    if (m == muc)
      return item;
  }
  return 0;
}

QTreeWidgetItem *MUCView::getUserItem(User *user)
{
  foreach (QTreeWidgetItem *item, tree->findItems(user->nick, Qt::MatchExactly | Qt::MatchRecursive, 0))
  {
    if (item->parent())
    {
      User *u = qVariantValue<User *> (item->data(0, Qt::UserRole));
      if (u == user)
        return item;
    }
  }
  return 0;
}

void MUCView::setUserStatus(User *user)
{
  QTreeWidgetItem *item = getUserItem(user);
  if (item)
  {
    if (user->status == Status::Offline)
      delete item;
    else
      item->setIcon(0, getIcon(user->status, user->role));
  }
  else
  {
    item = new QTreeWidgetItem(QStringList(user->nick), 0);
    item->setIcon(0, getIcon(user->status, user->role));
    item->setData(0, Qt::UserRole, qVariantFromValue(user));
    QTreeWidgetItem *muc = getMUCItem(user->muc);
    if (muc)
      muc->addChild(item);
  }
}

void MUCView::connectedToMUC(MUC *muc)
{
  QString msg(tr("You have been connected to \"%1\"").arg(muc->jid.bare()));
  m_makneto->getMaknetoMainWindow()->statusBar()->showMessage(msg, 2000);
  showInfoMessage(muc, msg);
  QTreeWidgetItem *item = getMUCItem(muc);
  if (item)
  {
    item->setIcon(0, iconOnline[0]);
  }

}

void MUCView::disconnectedFromMUC(MUC *muc)
{
  QString msg(tr("You have been disconnected from \"%1\"").arg(muc->jid.bare()));
  m_makneto->getMaknetoMainWindow()->statusBar()->showMessage(msg, 2000);
  showInfoMessage(muc, msg);
  QTreeWidgetItem *item = getMUCItem(muc);
  if (item)
  {
    item->setIcon(0, KIcon());
    item->takeChildren();
  }
}

bool MUCView::isMUCConnected(MUC *muc)
{
  QTreeWidgetItem *item = getMUCItem(muc);
  if (item)
    return item->childCount() != 0;
  return false;
}

bool MUCView::isMUCConnected(const Jid &jid)
{
  MUC *muc = m_control->getMUC(jid.bare());
  if (muc)
    return muc->users.count() != 0;
  return false;
}

void MUCView::error(MUC *muc, const QString &message)
{
  QString msg(tr("Error occured in MUC \"%1\": %2").arg(muc->jid.bare(), message));
  m_makneto->getMaknetoMainWindow()->statusBar()->showMessage(msg, 2000);
  showInfoMessage(muc, msg);
}

void MUCView::deletedMUC(MUC *muc)
{
  delete getMUCItem(muc);
}

void MUCView::showInfoMessage(MUC *muc, const QString &message)
{
  SessionView *session = m_maknetoview->getSessionTabManager()->findSession(muc->jid.bare());
  if (session)
    session->infoMessage(message);
}

void MUCView::joinMUC()
{
  QString nick, server, room;
  if (getConferenceSetting(room, server, nick))
  {
    // Create bookmark and add it to list
    MUC *muc = m_control->newMUC(QString("%1@%2/%3").arg(room, server, nick));
    m_control->saveMUCs();

    // Join to group chat
    emit connectToMUC(muc);
  }
}

void MUCView::itemDoubleClicked(QTreeWidgetItem *item, int)
{
  if (item->parent())
  {
    // User doubleclicked
  }
  else
  {
    // MUC doubleclicked
    MUC *muc = qVariantValue<MUC *>(item->data(0, Qt::UserRole));
    if (muc)
      emit connectToMUC(muc);
  }
}

KIcon MUCView::getIcon(const Status &status, const MUCItem::Role &role)
{
  KIcon *icon;
  switch (status.type())
  {
    case Status::Online:
      icon = iconOnline;
      break;
    case Status::Away:
      icon = iconAway;
      break;
    case Status::XA:
      icon = iconXA;
      break;
    case Status::DND:
      icon = iconDND;
      break;
    case Status::Invisible:
      icon = iconInvisible;
      break;
    case Status::FFC:
      icon = iconFFC;
      break;
    case Status::Offline:
      icon = iconOffline;
      break;
    default:
      return KIcon();
  }
  switch (role)
  {
    case MUCItem::Moderator:
      return icon[1];
      break;
    case MUCItem::Visitor:
      return icon[2];
      break;
    default:
      return icon[0];
      break;
  }
  return KIcon();
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
