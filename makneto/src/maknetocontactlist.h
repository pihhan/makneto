/*
 * maknetocontactlist.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETOCONTACTLIST_H
#define MAKNETOCONTACTLIST_H

#include "contactlist/contactlist.h"
#include "contactlist/status.h"

#include "featurelist.h"

namespace XMPP 
{
	class Status;
}

class Makneto;
class QActionGroup;

/**
 * This is contact list for Makneto
 *
 * @short Contact list class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class MaknetoContactList : public ContactList
{
public:
	/**
	* Default constructor
	*/
	MaknetoContactList(Makneto *makneto);

	/**
	* Destructor
	*/
	virtual ~MaknetoContactList() { }

	void addContact(const QString& name, const QString& jid, const QString& group);
	void setAvailability(const QString& jid, const QString &resource, const XMPP::Status& status);
	//void setUnavailability(const QString& jid);
        Makneto *makneto();

private:
	Makneto *m_makneto;
  QActionGroup *m_contactNewSessionActions, *m_contactDetailsActions;
        
};

#endif // MAKNETOCONTACTLIST_H
