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
#include <QtDebug>
#include <KIcon>

#include <iostream>


MaknetoContact::MaknetoContact(const QString& name, const QString& jid, 
        ContactListGroupItem* parent, QMenu *contactMenu
    ) :
      ContactListContact(parent), m_name(name), m_jid(jid), 
      m_status(ContactListStatus::Offline, "Offline"), 
      m_contactMenu(contactMenu)
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

QIcon MaknetoContactResource::statusIcon() const
{
	switch (status().type())
	{
    case ContactListStatus::Offline:
			return KIcon("maknetooffline");
			break;

    case ContactListStatus::Online:
			return KIcon("maknetoonline");
			break;

    case ContactListStatus::Away:
			return KIcon("maknetoaway");
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

QIcon MaknetoContact::statusIcon() const
{
    MaknetoContactResource r;
    r = bestResourceR();
    if (!r.isNull())
        return r.statusIcon();
    else
        return QIcon();
}



/*! \brief Create resource from first incoming presence. */
MaknetoContactResource::MaknetoContactResource(const XMPP::Status &status, const QString &resource)
    : ContactListContact(NULL),m_status(ContactListStatus::Offline),m_resource(0), m_null(false)
{
    m_resource = resource;
    setStatus(status);
    // TODO: caps hash checking a inicializace features.
}

MaknetoContactResource MaknetoContact::bestResourceR() const
{
    MaknetoContactResource p;
    ResourcesHash::const_iterator it;
    bool notused = true;
    for (it= m_resources.begin(); it != m_resources.end(); it++) {
        if (notused || p < *it) {
            p = *it;
            notused=false;
        }
    }
    return p;
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
    MaknetoContactResource resource;
    resource = bestResourceR();
    if (!resource.isNull()) {
        return resource.status();
    } else {
        return ContactListStatus(ContactListStatus::Offline); 
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

/** \brief Create new resource based on incoming presence. */
void MaknetoContact::createResource(const QString &resource, const XMPP::Status &status)
{
    MaknetoContactResource r;
    r = MaknetoContactResource(status, resource);
    m_resources.insert(resource, r);
    qDebug() << "Creating resource " << resource << " for contact "
        << jid();
    if (!status.capsVersion().isEmpty()) {
        qDebug() << "Contact has caps string " << status.capsVersion() << " and node " << status.capsNode();
    }
    if (m_contactMenu) {
        QAction *action= new QAction(resource, m_contactMenu);
        action->setData(QVariant(resource));

        m_contactMenu->addAction((action));
        QList<QAction *> actions = m_contactMenu->actions();
        for (QList<QAction *>::iterator it= actions.begin();
            it != actions.end(); it++) {
            qDebug() << "Menu Action: " << (*it)->text() << endl;
        }
    }
}

/** \brief Remove resource from contact. */
void MaknetoContact::removeResource(const QString &resource)
{
    m_resources.remove(resource);
    qDebug() << "Removing resource " << resource << " for contact "
        << jid();

    QAction *resourceAction = NULL;
    QList<QAction *> actions = m_contactMenu->actions();
    for (QList<QAction *>::iterator it= actions.begin();
        it != actions.end(); it++) {
        if ((*it)->text() == resource) {
            m_contactMenu->removeAction((*it));
            qDebug() << "Removing Menu Action: " << (*it)->text() << endl;
        }
    }
}

void MaknetoContact::setStatus(const QString &resource, const XMPP::Status &status)
{
    MaknetoContactResource r;
    r = this->resource(resource);
    if (!r.isNull()) {
        r.setStatus(status);
        if (status.type() == XMPP::Status::Offline) {
            removeResource(resource);
        }
    } else {
        // takove resource neexistuje, pokud neni offline, musim ho udelat
        if (status.type() != XMPP::Status::Offline) {
            createResource(resource, status);
        }
    }
}


QString MaknetoContact::resource() const
{
    QString r;
//            MaknetoContactResource *br = bestResource();
//            if (br)
//                r = br->resource();
    return r;
}

int MaknetoContact::priority() const
{
    int r = 0;
//            MaknetoContactResource *br = bestResource();
//            if (br)
//                r = br->priority();
    return r;
}

