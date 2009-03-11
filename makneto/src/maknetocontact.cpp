/*
 * maknetocontact.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "maknetocontact.h"
#include "contactlist/contactlistgroupitem.h"
#include "contactlist/status.h"

#include "xmpp_status.h"

#include <QtGui/QMenu>
#include <KIcon>

#include <iostream>

//KIcon MaknetoContact::statusIcons[7] =
KIcon MaknetoContact::statusIconOnline = KIcon("maknetoonline");
KIcon MaknetoContact::statusIconAway = KIcon("maknetoaway");
KIcon MaknetoContact::statusIconFFC = KIcon("maknetoonline");
KIcon MaknetoContact::statusIconDND = KIcon("maknetodnd");
KIcon MaknetoContact::statusIconXA = KIcon("maknetoxa");
KIcon MaknetoContact::statusIconOffline = KIcon("maknetooffline");
KIcon MaknetoContact::statusIconInvisible = KIcon("maknetoinvisible");

MaknetoContact::MaknetoContact(const QString& name, const QString& jid, ContactListGroupItem* parent, QMenu *contactMenu) : ContactListContact(parent), m_name(name), m_jid(jid), m_status(ContactListStatus::Offline, "Offline"), m_contactMenu(contactMenu)
{

}

MaknetoContact::~MaknetoContact()
{

}

MaknetoContact* MaknetoGroup::findContactByJid(const QString& jid)
{
	MaknetoContact *item = 0;
	bool found = false;
	int i = 0;

	while(!found && i<items())
	{
		item = static_cast<MaknetoContact*>(atIndex(i));
		
		if (item && item->jid() == jid)
			found = true;

		i++;
	}

	if (found)
		return item;
	else
		return 0;
}

void MaknetoContact::setStatus(const XMPP::Status &status)
{
	switch (status.type())
	{
		case XMPP::Status::Offline:
			m_status = ContactListStatus(ContactListStatus::Offline, "Offline");
			break;

		case XMPP::Status::Online:
			m_status = ContactListStatus(ContactListStatus::Online, "Online");
			break;

		case XMPP::Status::Away:
			m_status = ContactListStatus(ContactListStatus::Away, "Away");
			break;

		case XMPP::Status::XA:
			m_status = ContactListStatus(ContactListStatus::XA, "XA");
			break;

		case XMPP::Status::DND:
			m_status = ContactListStatus(ContactListStatus::DND, "DND");
			break;

		case XMPP::Status::Invisible:
			m_status = ContactListStatus(ContactListStatus::Invisible, "Invisible");
			break;

		case XMPP::Status::FFC:
			m_status = ContactListStatus(ContactListStatus::FFC, "Free for chat");
			break;
	}
}

void MaknetoContact::showContextMenu(const QPoint &where)
{
	m_contactMenu->exec(where);
}

QIcon MaknetoContact::statusIcon() const
{
  switch (status().type())
  {
    case ContactListStatus::Offline:
      return statusIconOffline;
      break;
    case ContactListStatus::Online:
      return statusIconOnline;
      break;
    case ContactListStatus::FFC:
      return statusIconOnline;
      break;
    case ContactListStatus::Away:
      return statusIconAway;
      break;
    case ContactListStatus::XA:
      return statusIconXA;
      break;
    case ContactListStatus::DND:
      return statusIconDND;
      break;
    case ContactListStatus::Invisible:
      return statusIconInvisible;
      break;
  }
  return QIcon();
}