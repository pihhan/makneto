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

#include <iostream>

void MaknetoContactList::addContact(const QString& name, const QString& jid, const QString& group = QString())
{
	ContactListGroupItem *groupItem = rootItem();

	// find/create group
	if (!group.isEmpty()) 
	{
		MaknetoGroup g(group);

		groupItem = static_cast<MaknetoGroup*>(rootItem()->findFirstItem(&g));

		if (!groupItem) 
		{
			groupItem = static_cast<MaknetoGroup*>(invisibleGroup()->findFirstItem(&g));
		}
	
		if (!groupItem) 
		{
			groupItem = new MaknetoGroup(group, rootItem());
		}
		
	}

	new MaknetoContact(name, jid, groupItem);
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
