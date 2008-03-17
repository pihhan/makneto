/*
 * maknetocontactlist.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETOCONTACTLIST_H
#define MAKNETOCONTACTLIST_H

#include "contactlist/contactlist.h"
#include "contactlist/status.h"

/**
 * This is contact list for Makneto
 *
 * @short Contact list class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class MaknetoContactList : public ContactList
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	MaknetoContactList() : ContactList() { }

	/**
	* Destructor
	*/
	virtual ~MaknetoContactList() { }

	void addContact(const QString& name, const QString& jid, const Status& status, const QString& group);

};

#endif // MAKNETOCONTACTLIST_H
