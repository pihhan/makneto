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
#include "contactlist/status.h"

#include <QtGui/QMenu>

/**
 * This is contact for Makneto
 *
 * @short Contact class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

namespace XMPP
{
	class Status;
}
class KIcon;

class MaknetoContact : public ContactListContact
{
public:
	/**
	* Default constructor
	*/
	MaknetoContact(const QString& name, const QString& jid, ContactListGroupItem* parent, QMenu *contactMenu);

	/**
	* Destructor
	*/
	virtual ~MaknetoContact();
	virtual const QString& name() const { return m_name; }
	virtual QString jid() const { return m_jid; }
	virtual ContactListStatus status() const { return m_status; }
	virtual void showContextMenu(const QPoint &where);

	void setName(const QString& name) { m_name = name; } 
	void setStatus(const XMPP::Status& status);
private:
	QString m_name;
	QString m_jid;
	ContactListStatus m_status;
	QMenu *m_contactMenu;
  QIcon statusIcon() const;
  static KIcon statusIcons[7];
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
	bool equals(ContactListItem *to) const
	{
		MaknetoGroup *group = dynamic_cast<MaknetoGroup*>(to);
		return (group && group->name() == m_name);
	}

	MaknetoContact* findContactByJid(const QString& jid);
private:
	QString m_name;
};

#endif // MAKNETOCONTACT_H
