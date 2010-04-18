
#include "contactlistgroupedcontact.h"
#include "maknetocontact.h"


ContactListGroupedContact::ContactListGroupedContact(ContactListGroupItem *parent, ContactListContact *contact)
    : ContactListContact(parent)
{
        m_contact = contact;

        MaknetoContact *mc = dynamic_cast<MaknetoContact *> (contact);
        if (mc) {
            connect(mc, SIGNAL(contactChanged()), this, SLOT(emitContactChanged()) );
        }
        
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

void ContactListGroupedContact::setGroup(ContactListGroupItem *)
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
    ContactListContact::updateParent();
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
        emit contactChanged(this);
}

