#ifndef CONTACTLISTCONTACT_H
#define CONTACTLISTCONTACT_H

#include <QString>
#include <QIcon>
#include <QHash>
#include <QList>

#include "status.h"
#include "contactlistitem.h"

class ContactListGroup;

class ContactListContact : public ContactListItem
{
public:
        typedef QHash<QString,ContactListStatus>    StatusHash;

	ContactListContact(ContactListGroupItem* defaultParent);
	virtual ~ContactListContact();

	virtual const QString name() const = 0;
	// FIXME: Change this into Jid
	virtual QString jid() const = 0;
	virtual ContactListStatus status() const = 0;
        virtual QString resource() const = 0;
        virtual int priority() const = 0;
	virtual QIcon statusIcon() const;
	virtual QIcon picture() const;
	virtual void updateParent();

	virtual QString toolTip() const;

	virtual int countOnline() const;

        typedef QList<ContactListGroup *> GroupsList;
        GroupsList getGroups();
        void    addGroup(ContactListGroup *group);
        void    removeGroup(ContactListGroup *group);
private:
        GroupsList  groups_;
        
};

#endif
