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

void MaknetoContactResource::setStatus(const XMPP::Status &status)
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
			return KIcon("maknetooffline");;
			break;

    case ContactListStatus::Online:
			return KIcon("maknetoonline");
			break;

    case ContactListStatus::Away:
			return KIcon("maknetoaway");;
			break;

    case ContactListStatus::XA:
			return KIcon("maknetoxa");
			break;

    case ContactListStatus::DND:
			return KIcon("maknetodnd");
			break;

    case ContactListStatus::Invisible:
			return KIcon("maknetoinvisible");
			break;

    case ContactListStatus::FFC:
			return KIcon("maknetoffc");
			break;
	}

	return QIcon();
}


/*! \brief Create resource from first incoming presence. */
MaknetoContactResource::MaknetoContactResource(const XMPP::Status &status, const QString &resource)
    : ContactListContact(NULL),m_status(ContactListStatus::Offline),m_resource(0)
{
    m_resource = resource;
    setStatus(status);
    // TODO: caps hash checking a inicializace features.
}

MaknetoContactResource * MaknetoContact::bestResource() 
{
    MaknetoContactResource *p = NULL;
    ResourcesHash::iterator it;
    for (it= m_resources.begin(); it != m_resources.end(); it++) {
        if (!p || *p < *it) {
            p = &(*it);
        }
    }
    return p;
}

ContactListStatus MaknetoContact::status() const 
{ 
    MaknetoContactResource *resource;
    resource = bestResource();
    if (resource != NULL) {
        return resource->status();
    } else {
        return ContactListStatus(Offline); 
    }
}

void MaknetoContact::setStatus(const XMPP::Status &status)
{
    MaknetoContactResource *resource;
    resource = bestResource();
    if (resource != NULL) {
        resource->setStatus(status);
    }
}


