/*
 * maknetocontactlist.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETOCONTACTLIST_H
#define MAKNETOCONTACTLIST_H

#include "contactlist/contactlist.h"
#include "contactlist/status.h"

#include <QObject>

/**
 * This is contact list for Makneto
 *
 * @short Contact list class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

namespace XMPP 
{
	class Status;
}

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

	void addContact(const QString& name, const QString& jid, const QString& group);
	void setAvailability(const QString& jid, const XMPP::Status& status);
	//void setUnavailability(const QString& jid);
};

#endif // MAKNETOCONTACTLIST_H
