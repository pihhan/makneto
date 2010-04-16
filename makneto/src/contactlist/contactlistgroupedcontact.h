
#ifndef CONTACTLISTGROUPEDCONTACT_H
#define CONTACTLISTGROUPEDCONTACT_H

#include "contactlistcontact.h"
#include "contactlistgroupitem.h"

/*! \brief Class for wrapping contact together with reference to group it is inside.
 * \author Petr Mensik <pihhan@cipis.net>
 *  Intented only for displaying by model, not network operations. It should replace
 *  current use of MaknetoContact in model roster, so it can be in multiple places
 *  inside roster, with still single MaknetoContact pointed to by this proxy.
 *  */
class ContactListGroupedContact : public ContactListContact
{
        Q_OBJECT
        public:

        ContactListGroupedContact(ContactListGroupItem *parent, ContactListContact *contact);

        ContactListContact *contact() const;
        ContactListGroupItem *group() const;

        void setContact(ContactListContact *contact);
        void setGroup(ContactListGroupItem *item);

	virtual const QString name() const;
	virtual QString jid() const;
	virtual ContactListStatus status() const;
        virtual QString resource() const;
        virtual int priority() const;
	virtual QIcon statusIcon() const;
	virtual QIcon picture() const;
	virtual void updateParent();

	virtual QString toolTip() const;

	virtual int countOnline() const;

    virtual bool supportsFeature(const QString &string) const;

        public Q_SLOTS:
        void emitContactChanged();

        Q_SIGNALS:
        void contactChanged(ContactListGroupedContact *);

        private:
        ContactListContact *m_contact;
        ContactListGroupItem *m_group;
};


#endif

