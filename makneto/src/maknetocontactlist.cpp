/*
 * maknetocontactlist.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "maknetocontactlist.h"
#include "maknetocontact.h"
#include "contactlist/contactlistgroupitem.h"
#include "contactlist/contactlistrootitem.h"

void MaknetoContactList::addContact(const QString& name, const QString& jid, const Status& status, const QString& group = QString())
{
	ContactListGroupItem *groupItem = rootItem();

	if (!group.isEmpty()) 
	{
		MaknetoGroup g(group);
		groupItem = static_cast<MaknetoGroup*>(rootItem()->findFirstItem(&g));;

		if (!groupItem) 
		{
			groupItem = static_cast<MaknetoGroup*>(invisibleGroup()->findFirstItem(&g));
		}
	
		if (!groupItem) 
		{
			groupItem = new MaknetoGroup(group, rootItem());
		}
		
	}

	new MaknetoContact(name, jid, status, groupItem);
}
