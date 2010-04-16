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
#include "contactlist/contactlistgroupedcontact.h"
#include "contactlist/status.h"
#include "featurelist.h"

#include <QtGui/QMenu>

#include <xmpp_status.h>

class KIcon;
class MaknetoContact;

/*! \brief Small class for one resource of one contact.
 *
 * It will keep all information about connected entity. It's features, it's
 * identification, maybe version. 
 * TODO: Should it hold avatar, if supported? Can avatar differ between 
 * resources? 
 * \author Petr Mensik <pihhan@cipis.net> */
class MaknetoContactResource : public ContactListContact 
{
    Q_OBJECT
public:
        MaknetoContactResource();

        MaknetoContactResource(MaknetoContact *bare, const QString &resource, int priority = 0);

        MaknetoContactResource(MaknetoContact *bare, const XMPP::Status &status, const QString &resource);

        virtual QString jid() const;
        virtual QString resource() const { return m_resource; }
        virtual int priority() const { return m_priority; }
        virtual FeatureList *features() { return m_features; } 
	virtual ContactListStatus status() const { return m_status; }
        /*! \brief only compatibility with ContactListContact, name is the same for all resources, no need to store it for every single resource. */
        virtual const QString name() const { return QString(); } 
	virtual void setStatus(const XMPP::Status& status);
        virtual void setPriority(int prio) { m_priority = prio; }
        virtual void setResource(const QString &resource) { m_resource = resource; m_null=true; }

        virtual void updateParent();

        virtual bool supportsFeature(const QString &feature) const;
        bool isNull() const { return m_null; }

        bool operator<(const MaknetoContactResource &other) const
        {
            return (m_resource < other.m_resource);
        }

        void setFeatures( FeatureList *fl, bool shared=true);

        QIcon statusIcon() const;

        bool supportsVideo() const;
        bool supportsAudio() const;
        bool supportsWhiteboard() const; 
private: 
        MaknetoContact  *m_bare;
	ContactListStatus m_status;
	QMenu   *m_contactMenu;
        QString m_resource;
        int     m_priority;
        FeatureList *m_features;
        bool    m_feature_shared;
        QString m_clientname;
        bool    m_null;
};


/**
 * This is contact for Makneto
 *
 * @short Contact class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @author Petr Mensik <petr-mensik@seznam.cz>
 * @version 0.2
 */

class MaknetoContact : public ContactListContact
{
    Q_OBJECT
public:

        typedef QHash<QString, MaknetoContactResource *>   ResourcesHash;
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
    
        virtual void updateParent();

        virtual MaknetoContactResource *resource( const QString &resource) 
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
        void setStatus(const QString &resource, const XMPP::Status &status, FeatureListManager *flm=NULL);
        /*! \brief Create new resource entry using its name na incoming status. */
        MaknetoContactResource * createResource(const QString &resource, const XMPP::Status &status);
        void removeResource(const QString &resource);

        virtual bool supportsFeature(const QString &feature) const;

        bool supportsVideo() const;
        bool supportsAudio() const;
        bool supportsWhiteboard() const;
        bool isOnline() const;

        ResourcesHash allResources()
        { return m_resources; }

signals:
    void contactChanged();

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
	MaknetoGroup(const QString& name, ContactListGroupItem* parent = 0);

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

//	MaknetoContact* findContactByJid(const QString& jid);
	ContactListGroupedContact* findContactByJid(const QString& jid);
private:
	QString m_name;
};

#endif // MAKNETOCONTACT_H
