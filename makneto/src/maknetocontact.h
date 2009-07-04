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

#include <xmpp_status.h>

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
        MaknetoContactResource()
            : ContactListContact(NULL),m_status(ContactListStatus::Offline),
              m_contactMenu(NULL), 
              m_priority(0), m_null(true)
        {}

        MaknetoContactResource(const QString &resource, int priority = 0)
            : ContactListContact(NULL),m_status(ContactListStatus::Offline),
              m_contactMenu(NULL), 
              m_resource(resource),m_priority(priority), m_null(false)
        {}

        MaknetoContactResource(const XMPP::Status &status, const QString &resource);

        virtual QString jid() const { return ""; }
        virtual QString resource() const { return m_resource; }
        virtual int priority() const { return m_priority; }
        virtual FeatureList features() { return m_features; } 
	virtual ContactListStatus status() const { return m_status; }
        /*! \brief only compatibility with ContactListContact, name is the same for all resources, no need to store it for every single resource. */
        virtual const QString name() const { return QString(); } 
	virtual void setStatus(const XMPP::Status& status);
        virtual void setPriority(int prio) { m_priority = prio; }
        virtual void setResource(const QString &resource) { m_resource = resource; m_null=true; }
        bool isNull() const { return m_null; }

        bool operator<(const MaknetoContactResource &other) const
        {
            return (m_resource < other.m_resource);
        }

        QIcon statusIcon() const;
private: 
	ContactListStatus m_status;
	QMenu *m_contactMenu;
        QString m_resource;
        int m_priority;
        FeatureList m_features;
        QString m_clientname;
        bool m_null;
};


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

        typedef QHash<QString, MaknetoContactResource>   ResourcesHash;
	/**
	* Default constructor
	*/
	MaknetoContact(const QString& name, const QString& jid, ContactListGroupItem* parent, QMenu *contactMenu);

	/**
	* Destructor
	*/
	virtual ~MaknetoContact();
	virtual const QString name() const { return m_name; }
	virtual QString jid() const { return m_jid; }
	virtual ContactListStatus status() const;
	virtual void showContextMenu(const QPoint &where);

        virtual QString resource() const;

        virtual int priority() const;

        virtual MaknetoContactResource resource( const QString &resource) 
        {
            return m_resources.value(resource);
        }
        /*! \brief Return resource with highest priority. 
         * \return Pointer to best resource class or NULL, if none is present - offline contact. */
        virtual MaknetoContactResource *bestResource() ;
        /*! \brief Return resource with highest priority.
         * \return Copy of MaknetoContactResource. it will return isNull(), if not really online. */
        virtual MaknetoContactResource bestResourceR() const;
        int resourcesNumber() const { return m_resources.size(); }

	void setName(const QString& name) { m_name = name; } 
	void setStatus(const XMPP::Status& status);
        /*! \brief Set status for specified resource.
         * Create new one if it does not exist and status is not offline,
         * or dispose existing one if status is offline. */
        void setStatus(const QString &resource, const XMPP::Status &status);
        /*! \brief Create new resource entry using its name na incoming status. */
        void createResource(const QString &resource, const XMPP::Status &status);
        void removeResource(const QString &resource);

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

/*! \brief Implementation of both graphical and logical part of group in roster. */
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
