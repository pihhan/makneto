#include <QPixmap>
#include <QDebug>

#include "contactlist.h"
#include "contactlistgroupitem.h"
#include "contactlistrootitem.h"
#include "contactlistcontact.h"
#include "status.h"

ContactListContact::ContactListContact(ContactListGroupItem* parent) : ContactListItem(parent)
{
}

ContactListContact::~ContactListContact()
{
}

int ContactListContact::countOnline() const
{
	return (status().type() == ContactListStatus::Offline ? 0 : 1);
}

QString ContactListContact::toolTip() const
{
	QString txt;

	txt += "<table><tr><td>";
	//txt += "<img src=\":/tooltip/icon.png\"/>";
	txt += "</td><td>";
	txt += name() + "<br/>";
	txt += "[" + jid() + "]<br/>" ;
	txt += status().message();
	txt += "</tr></td></table>";
	return txt;
}

QIcon ContactListContact::picture() const
{
	return QIcon();
}

QIcon ContactListContact::statusIcon() const
{
	return QIcon();
}

/*! \brief Update display status of this contact and move it to displaygroup.
 * 
 * Display group can be hidden, or instance of real roster group. It depends
 * on offline contacts enable, search mode and other roster parameters. 
 */
void ContactListContact::updateParent()
{
	ContactListGroupItem* newParent = parent();
	if (!contactList()->search().isEmpty()) {
		QString search = contactList()->search();
		if (name().contains(search) || jid().contains(search)) {
			newParent = defaultParent();
		}
		else {
			newParent = contactList()->invisibleGroup();
		}
	}
	else if (status().type() == ContactListStatus::Offline && !contactList()->showOffline()) {
#ifdef CONTACTLIST_DEBUG
		qDebug() << "contactlistcontact.cpp: Contact is offline and not shoved: " << jid();
#endif
		newParent = contactList()->invisibleGroup();
	}
	else {
#ifdef CONTACTLIST_DEBUG
		qDebug() << "contactlistcontact.cpp: Falling back on default parent: " << jid();
#endif
		newParent = defaultParent();
	}

	if (!contactList()->showGroups() && newParent != contactList()->invisibleGroup()) {
		newParent = contactList()->rootItem();
	}

        if (contactList()->showOnlyAudio() && !supportsAudio()) {
                newParent = contactList()->invisibleGroup();
        }

        if (contactList()->showOnlyVideo() && !supportsVideo()) {
                newParent = contactList()->invisibleGroup();
        }

	if (newParent != parent())
		setParent(newParent);
}

/*! \brief Add group this contact is member of.
    \param Pointer to ContactListGroup.
    It does not modify group itself, it only adds that group to list,
    if it is not already in list. In that case, it does nothing. 
*/
void    ContactListContact::addGroup(ContactListGroup *group)
{
    GroupsList::iterator it = groups_.begin();
    for ( ; it != groups_.end(); ++it) {
        if (*it == group) // already in list
            return;
    }
    groups_.append(group);
}

/*! \brief Remove group this contact is member of.
    \param group Pointer to group.
    It will not remove contact from group itself, it only modifies list
    of groups in this class. 
*/
void    ContactListContact::removeGroup(ContactListGroup *group)
{
    groups_.removeAll(group);
}


/*! \brief Check whether contact supports extension with given XML namespace. 
 *  \param feature Text namespace to check
 *  \return true if supported, false otherwise. */
bool ContactListContact::supportsFeature(const QString &) const
{
    return false;
}


bool ContactListContact::supportsAudio() const
{
    return false;
}

bool ContactListContact::supportsVideo() const
{
    return false;
}

bool ContactListContact::supportsWhiteboard() const
{
    return false;
}

