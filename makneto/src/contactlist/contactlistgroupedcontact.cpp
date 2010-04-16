
#include "contactlistgroupedcontact.h"


ContactListGroupedContact::ContactListGroupedContact(ContactListItem *parent, ContactListContact *contact)
    : ContactListItem(parent)
{
        m_contact = contact;
}


ContactListGroupItem * ContactListGroupedContact::group() const
{
        return dynamic_cast<ContactListGroupItem *> (parent());
}

ContactListContact * ContactListGroupedContact::contact() const
{
        return m_contact;
}

void ContactListGroupedContact::setContact(ContactListContact *contact)
{
        m_contact = contact;
}

void ContactListGroupedContact::setGroup(ContactListGroupItem *item)
{
}

const QString ContactListGroupedContact::name() const
{
        return m_contact->name();
}

// FIXME: Change this into Jid
QString ContactListGroupedContact::jid() const
{
        return m_contact->jid();
}
ContactListStatus ContactListGroupedContact::status() const
{
        return m_contact->status();
}

QString ContactListGroupedContact::resource() const
{
        return m_contact->resource();
}

int ContactListGroupedContact::priority() const
{
        return m_contact->priority();
}

QIcon ContactListGroupedContact::statusIcon() const
{
        return m_contact->statusIcon();
}

QIcon ContactListGroupedContact::picture() const
{
        return m_contact->picture();
}

void ContactListGroupedContact::updateParent()
{
        return m_contact->updateParent();
}

QString ContactListGroupedContact::toolTip() const
{
        return m_contact->toolTip();
}

int ContactListGroupedContact::countOnline() const
{
        return m_contact->countOnline();
}

bool ContactListGroupedContact::supportsFeature(const QString &string) const
{
        return m_contact->supportsFeature(string);
}
    
void ContactListGroupedContact::emitContactChanged()
{
        emit contactChanged();
}

