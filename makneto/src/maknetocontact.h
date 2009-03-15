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
#include "featurelist.h"

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

/*! \brief Small class for one resource of one contact.
 *
 * It will keep all information about connected entity. It's features, it's
 * identification, maybe version. 
 * TODO: Should it hold avatar, if supported? Can avatar differ between 
 * resources? 
 * \author Petr Mensik <pihhan@cipis.net> */
class MaknetoContactResource : public ContactListContact
{
public:
        MaknetoContactResource(const QString &resource, int priority = 0)
            : ContactListContact(NULL),m_status(ContactListStatus::Offline),
              m_contactMenu(NULL), 
              m_resource(resource),m_priority(priority)
        {

        }

        MaknetoContactResource(const XMPP::Status &status);

        virtual QString jid() const { return ""; }
        virtual QString resource() const { return m_resource; }
        virtual int priority() const { return m_priority; }
        virtual FeatureList features() { return m_features; } 
	virtual ContactListStatus status() const { return m_status; }
        /*! \brief only compatibility with ContactListContact, name is the same for all resources, no need to store it for every single resource. */
        virtual const QString & name() const { return QString(); } 
	void setStatus(const XMPP::Status& status);
        void setPriority(int prio) { m_priority = prio; }
private: 
	ContactListStatus m_status;
	QMenu *m_contactMenu;
        QString m_resource;
        int m_priority;
        QIcon statusIcon() const;
        FeatureList m_features;
        QString m_clientname;
};

class MaknetoContact : public ContactListContact
{
public:

        typedef QHash<QString, MaknetoContactResource>   ResourcesHash;
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
	virtual ContactListStatus status() const;
	virtual void showContextMenu(const QPoint &where);
        virtual MaknetoContactResource resource( const QString &resource) 
        {
            return m_resources.value(resource);
        }
        /*! \brief Return resource with highest priority. 
         * \return Pointer to best resource class or NULL, if none is present - offline contact. */
        virtual MaknetoContactResource *bestResource();
        int resourcesNumber() const { return m_resources.size(); }

	void setName(const QString& name) { m_name = name; } 
	void setStatus(const XMPP::Status& status);
private:
	QString m_name;
	QString m_jid;
	ContactListStatus m_status;
	QMenu *m_contactMenu;
        QIcon statusIcon() const;
        static KIcon statusIconOnline, statusIconAway, statusIconFFC, statusIconDND, statusIconXA, statusIconOffline, statusIconInvisible;
        ResourcesHash m_resources;
        ContactListGroupItem *m_parent;
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
