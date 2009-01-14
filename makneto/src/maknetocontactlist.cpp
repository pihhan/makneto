/*
 * maknetocontactlist.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "maknetocontactlist.h"
#include "maknetocontact.h"
#include "contactlist/contactlistgroupitem.h"
#include "contactlist/contactlistrootitem.h"

#include "xmpp_status.h"

#include "makneto.h"

#include <QtGui/QMenu>
#include <QtGui/QActionGroup>
#include <QtGui/QAction>

#include <iostream>

MaknetoContactList::MaknetoContactList(Makneto *makneto) : ContactList(), m_makneto(makneto) 
{ 
	m_contactActions = new QActionGroup(0);
	connect(m_contactActions, SIGNAL(triggered(QAction *)), makneto, SLOT(contactTriggered(QAction *)));
}

void MaknetoContactList::addContact(const QString& name, const QString& jid, const QString& group = QString())
{
	ContactListGroupItem *groupItem = rootItem();
	QMenu *contactMenu = new QMenu(NULL);
	QString groupCorrected;

	if (group.isEmpty())
		groupCorrected = "Unknown";
	else
		groupCorrected = group;

	// find/create group
	if (!groupCorrected.isEmpty()) 
	{
		MaknetoGroup g(groupCorrected);

		groupItem = static_cast<MaknetoGroup*>(rootItem()->findFirstItem(&g));

		if (!groupItem) 
		{
			groupItem = static_cast<MaknetoGroup*>(invisibleGroup()->findFirstItem(&g));
		}
	
		if (!groupItem) 
		{
			groupItem = new MaknetoGroup(groupCorrected, rootItem());
		}
	}

	// create contact menu
	QAction *newSession = new QAction(i18n("New &session"), m_contactActions);
	newSession->setData(QVariant(jid));
  
  QAction *contactDetails = new QAction(i18n("&Contact details"), m_contactActions);
  contactDetails->setData(QVariant(jid));

	//connect(contactDetails, SIGNAL(triggered()), this, SLOT(contactDetails()));

	contactMenu->addAction(newSession);
  contactMenu->addAction(contactDetails);

	new MaknetoContact(name, jid, groupItem, contactMenu);
}

void MaknetoContactList::setAvailability(const QString& jid, const XMPP::Status& status)
{
	std::cout << "setAvailability(" << jid.toUtf8().data() << ")" << std::endl;

	ContactListGroupItem *root = rootItem();

	MaknetoGroup *groupItem = 0;
	MaknetoContact *item = 0;

	for (int i=0; i<root->items(); i++)
	{
		groupItem = static_cast<MaknetoGroup*>(root->atIndex(i));
		item = 0;

		if (groupItem)
		{
			item = groupItem->findContactByJid(jid);

			if (item)
			{
				item->setStatus(status);
			}
		}
	}

	emitDataChanged();
}
#include "maknetocontactlist.moc"
