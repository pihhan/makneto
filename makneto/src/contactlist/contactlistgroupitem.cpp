#include <QList>

#include "contactlist.h"
#include "contactlistrootitem.h"
#include "contactlistgroupitem.h"
#include "contactlistitemcomparator.h"

ContactListGroupItem::ContactListGroupItem(ContactListGroupItem* parent) : ContactListItem(parent)
{
}

ContactListGroupItem::~ContactListGroupItem()
{
	while (!items_.isEmpty())
		delete items_.takeFirst();
}

int ContactListGroupItem::indexOf(ContactListItem* item) const
{
	return items_.indexOf(item);
}

/*! \brief Get item at specified position.
    \param index Position of requested contact
    \return Pointer to item or NULL, if not specified.
    */
ContactListItem* ContactListGroupItem::atIndex(int index) const
{
	if (index >= 0 && index < items_.size())
		return items_[index];
	else
		return 0;
}

int ContactListGroupItem::items() const
{
	return items_.size();
}

/*! \brief Add new contact to group. */
void ContactListGroupItem::addItem(ContactListItem* item)
{
	QList<ContactListItem*>::Iterator it = items_.begin();
	bool inserted = false;
	while(it != items_.end() && !inserted) {
		if (contactList()->itemComparator()->compare(*it,item) >= 0) {
			items_.insert(it,item);
			inserted = true;
		}
		it++;
	}

	if (!inserted) {
		items_.push_back(item);
	}

	updateParent();
}

void ContactListGroupItem::removeItem(ContactListItem* item)
{
	items_.removeAll(item);
	updateParent();
}

ContactListItem* ContactListGroupItem::findFirstItem(ContactListItem* other)
{
	if (equals(other)) {
		return this;
	}
	else {
		ContactListItem* item;
		for (int i = 0; i < items_.size(); ++i) {
			item = items_.at(i)->findFirstItem(other);
			if (item)
				return item;
		}
		return 0;
	}
}

bool ContactListGroupItem::expanded() const
{
	return true;
}

int ContactListGroupItem::count() const
{
	int count = 0;
	for (int i = 0; i < items_.size(); ++i) {
		count += items_.at(i)->count();
	}
	return count;
}

int ContactListGroupItem::countOnline() const
{
	int count = 0;
	for (int i = 0; i < items_.size(); ++i) {
		count += items_.at(i)->countOnline();
	}
	return count;
}

/*! \brief Move this item to hidden group or to real parent. */
void ContactListGroupItem::updateParent()
{
	ContactListGroupItem* newParent = parent();
	if (items_.size() == 0 || !contactList()->showGroups()) {
		newParent = contactList()->invisibleGroup();
	}
	else {
		newParent = defaultParent();
	}

	if (newParent != parent()) {
		setParent(newParent);
	}
}

/*! \brief Update parents of every item in this group, also group item itself. */
void ContactListGroupItem::updateParents()
{
	QList<ContactListItem*> items(items_);
	for (int i = 0; i < items.size(); ++i) {
		items.at(i)->updateParents();
	}
	updateParent();
}

/*QList<ContactListItem*> ContactListGroupItem::visibleItems()
{
	QList<ContactListItem*> list;
	for (int i = 0; i < items_.size(); ++i) {
		list += items_.at(i)->visibleItems();
	}
	return list;
}

QList<ContactListItem*> ContactListGroupItem::invisibleItems()
{
	QList<ContactListItem*> list;
	for (int i = 0; i < items_.size(); ++i) {
		list += items_.at(i)->invisibleItems();
	}
	return list;
}*/
