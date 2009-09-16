#include "contactlist.h"
#include "contactlistitem.h"
#include "contactlistgroupitem.h"

ContactListItem::ContactListItem(ContactListGroupItem* parent) : QObject(), parent_(NULL), defaultParent_(parent), contactlist_(NULL)
{
	setParent(parent);
}

ContactListItem::ContactListItem(const ContactListItem &item)
    : QObject(), parent_(NULL),defaultParent_(item.defaultParent_), contactlist_(NULL)
{
        setParent(item.parent_);
}

void ContactListItem::operator=(const ContactListItem &item)
{
    defaultParent_ = item.defaultParent_;
    setParent(item.parent_);
}

/*! \brief Get contact list instance. */
ContactList* ContactListItem::contactList() const
{
        if (contactlist_) {
            return contactlist_;
        } else if (parent()) {
		return parent()->contactList();
	}
	else {
		qWarning("contaclistitem.cpp: Unable to determine contactlist");
		return NULL;
	}
}

/*! \brief Get item parent. */
ContactListGroupItem* ContactListItem::parent() const
{
	return parent_;
}

/*! \brief Get default item parent. */
ContactListGroupItem* ContactListItem::defaultParent() const
{
	return defaultParent_;
}

/*! \brief Set new item parent.
    \param parent New parent item. */
void ContactListItem::setParent(ContactListGroupItem* parent)
{
	if (parent_ != parent) {
		if (parent_) {
			parent_->removeItem(this);
		}
		
		if (parent) {
			parent->addItem(this);
                        contactlist_ = parent->contactList();
		}
		
		parent_ = parent;
		contactList()->emitDataChanged(this);
	}
}

/*! \brief Get this item index in parent structure. */
int ContactListItem::index() 
{
	return (parent() ? parent()->indexOf(this) : 0);
}


bool ContactListItem::isVisible() const
{
	return true;
}

bool ContactListItem::equals(ContactListItem* other) const
{
	return (this == other);
}

QString ContactListItem::toolTip() const
{
	return QString();
}

void ContactListItem::updateParent()
{
}

void ContactListItem::updateParents()
{
	updateParent();
}

ContactListItem* ContactListItem::findFirstItem(ContactListItem* other)
{
	return (equals(other) ? this : NULL);
}

int ContactListItem::count() const
{
	return 1;
}

int ContactListItem::countOnline() const
{
	return 0;
}

/*QList<ContactListItem*> ContactListItem::visibleItems()
{
	QList<ContactListItem*> l;
	if (isVisible())
		l += this;
	return l;
}

QList<ContactListItem*> ContactListItem::invisibleItems()
{
	QList<ContactListItem*> l;
	if (!isVisible())
		l += this;
	return l;
}*/

/*! \brief Not implemented. */
void ContactListItem::showContextMenu(const QPoint&)
{
}
