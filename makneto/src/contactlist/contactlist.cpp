#include "contactlist.h"
#include "contactlistgroupitem.h"
#include "contactlistrootitem.h"
#include "contactlistalphacomparator.h"
#include "contactlistcontact.h"
#include "contactlistgroup.h"

#include <QDebug>


ContactList::ContactList(QObject* parent)
	: QObject(parent), showOffline_(false), showGroups_(true),
            showAudio_(false), showVideo_(false)
{
	rootItem_ = new ContactListRootItem(this);
	invisibleGroup_ = new ContactListRootItem(this);
	altInvisibleGroup_ = new ContactListRootItem(this);
    contactRootGroup_ = new ContactListRootItem(this);
	itemComparator_ = new ContactListAlphaComparator();
}

const ContactListItemComparator* ContactList::itemComparator() const
{
	return itemComparator_;
}

const QString& ContactList::search() const
{
	return search_;
}

void ContactList::setShowOffline(bool showOffline)
{
	if (showOffline_ != showOffline) {
		showOffline_ = showOffline;
		updateParents();
	}
}

void ContactList::setShowGroups(bool showGroups)
{
	if (showGroups_ != showGroups) {
		showGroups_ = showGroups;
		updateParents();
	}
}

void ContactList::setSearch(const QString& search)
{
	QString oldSearch = search_;
	search_ = search;

	if ((oldSearch.isEmpty() && !search.isEmpty()) || search.isEmpty()) {
		updateParents();
	}
	if (search.isEmpty()) {
		updateParents();
	}
	else if (search.startsWith(oldSearch)) {
		updateVisibleParents();
	}
	else if (oldSearch.startsWith(search)) {
		updateInvisibleParents();
	}
	else {
		updateParents();
	}
}

void ContactList::emitDataChanged() 
{
	emit dataChanged();
}

/*! \brief Update only specified item. */
void ContactList::emitDataChanged(ContactListItem *item) 
{
	emit dataChangedItem(item);
}

/*void ContactList::setShowGroups(bool showGroups)
{
	showGroups_ = showGroups;
}

void ContactList::setShowAccounts(bool showAccounts)
{
	showAccounts_ = showAccounts;
}*/

/*! \brief Get root item of contact list, where all items are nested. */
ContactListRootItem* ContactList::rootItem()
{
	return rootItem_;
}

ContactListRootItem* ContactList::invisibleGroup()
{
	return invisibleGroup_;
}

/*ContactListGroupItem* ContactList::hiddenGroup()
{
	return hiddenGroup_;
}

ContactListGroupItem* ContactList::agentsGroup()
{
	return agentsGroup_;
}

ContactListGroupItem* ContactList::conferenceGroup()
{
	return conferenceGroup_;
}*/

void ContactList::updateVisibleParents()
{
	rootItem()->updateParents();
	emit dataChanged();
}

void ContactList::updateInvisibleParents()
{
	invisibleGroup()->updateParents();
	emit dataChanged();
}

void ContactList::updateParents()
{
	// Switch invisible groups
	ContactListRootItem* tmpInvisibleGroup = invisibleGroup_;
	invisibleGroup_ = altInvisibleGroup_;
	altInvisibleGroup_ = tmpInvisibleGroup;

	// Move items around
	rootItem()->updateParents();
	altInvisibleGroup_->updateParents();

#ifdef CONTACTLIST_DEBUG
        qDebug() << "root items: " << rootItem()->items()
            << " invisible items: " << invisibleGroup_->items()
            << " alt invisible items: " << altInvisibleGroup_->items();
#endif

	emit dataChanged();
}

/*! \brief Get contact class by jid.
    \param jid Bare jid of contact.
    \return Pointer to jid on success, NULL if no such contact exists in roster. 
*/
ContactListContact * ContactList::getContact(const QString &jid)
{
    ContactsHash::iterator it;
    it = contacts_.find(jid);
    if (it != contacts_.end())
        return *it;
    else 
        return NULL;
}

/*! \brief get Group class by name.
    \param groupname Name of group.
    \return Pointer to group on success, NULL if no such group exists.
*/
ContactListGroupItem * ContactList::getGroup(const QString &groupname)
{
    GroupsHash::iterator it;
    it = groups_.find(groupname);
    if (it != groups_.end())
        return *it;
    else 
        return NULL;
}

/*! \brief Create new group of specified name, or get pointer to existing 
    group item if exist.
*/
ContactListGroupItem * ContactList::addGroup(const QString &name)
{
    ContactListGroupItem *g = getGroup(name);
    if (g) {
        qWarning() << "request to add group " << name << " to roster, but it already exists.";
        return g;
    } else {
        ContactListGroupItem *gi = new ContactListGroupItem(rootItem_);
        addItem(gi);
        return gi;
    }
}

/*! \brief Add item to contact list. 
    It will add group or contact to hash, depending on its type,
    so it can be get faster for 
    modification later by jid. */
void ContactList::addItem(ContactListItem *item)
{
    ContactListContact *contact = dynamic_cast<ContactListContact *>(item);
    ContactListGroup *group = dynamic_cast<ContactListGroup *>(item);

    if (contact != NULL) {
        // TODO: add to all groups it might be member of also
        contacts_.insert(contact->jid(), contact);
    } else if (group != NULL) {
        groups_.insert(group->name(), group);
    }
}

void ContactList::removeItem(ContactListItem *item)
{
    ContactListContact *contact = dynamic_cast<ContactListContact *>(item);
    ContactListGroup *group = dynamic_cast<ContactListGroup *>(item);

    if (contact != NULL) {
        contacts_.remove(contact->jid());
    } else if (group != NULL) {
        groups_.remove(group->name());
    }
}

QStringList ContactList::allGroupNames() const
{
    QStringList groups;

    for (GroupsHash::const_iterator it=groups_.begin(); it!=groups_.end(); it++) 
        groups.append((*it)->name());
    return groups;
}

ContactList::GroupsHash  ContactList::allGroups() const
{
    return groups_;
}

ContactList::ContactsHash ContactList::allContacts() const
{
    return contacts_;
}

unsigned int ContactList::groupCount() const
{
    return  groups_.size();
}

unsigned int ContactList::contactCount() const
{
    return contacts_.size();
}

ContactListRootItem* ContactList::contactRoot()
{
    return contactRootGroup_;
}

bool ContactList::showOnlyAudio() const
{
    return showAudio_;
}

bool ContactList::showOnlyVideo() const
{
    return showVideo_;
}

void ContactList::setShowOnlyAudio(bool show)
{
    showAudio_ = show;
    updateParents();
}

void ContactList::setShowOnlyVideo(bool show)
{
    showVideo_ = show;
    updateParents();
}

