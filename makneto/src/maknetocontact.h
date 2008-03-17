/*
 * maknetocontact.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETOCONTACT_H
#define MAKNETOCONTACT_H

#include "contactlist/contactlistcontact.h"
#include "contactlist/contactlistgroup.h"
#include "contactlist/contactlistgroupitem.h"

/**
 * This is contact for Makneto
 *
 * @short Contact class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class MaknetoContact : public ContactListContact
{
public:
	/**
	* Default constructor
	*/
	MaknetoContact(const QString& name, const QString& jid, const Status& status, ContactListGroupItem* parent);

	/**
	* Destructor
	*/
	virtual ~MaknetoContact();
	virtual const QString& name() const { return m_name; }
	virtual QString jid() const { return m_jid; }
	virtual Status status() const { return m_status; }
private:
	QString m_name;
	QString m_jid;
	Status m_status;
};

class MaknetoGroup : public ContactListGroup
{
public:
	/**
	* Default constructor
	*/
	MaknetoGroup(const QString& name, ContactListGroupItem* parent = 0): ContactListGroup(parent), m_name(name) { }

	/**
	* Destructor	
	*/
	virtual ~MaknetoGroup() { }

	virtual const QString& name() const { return m_name; } 
private:
	QString m_name;
};

#endif // MAKNETOCONTACT_H
