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
		//qDebug() << "contactlistcontact.cpp: Contact is invisible";
		newParent = contactList()->invisibleGroup();
	}
	else {
		//qDebug() << "contactlistcontact.cpp: Falling back on default parent";
		newParent = defaultParent();
	}

	if (!contactList()->showGroups() && newParent != contactList()->invisibleGroup()) {
		newParent = contactList()->rootItem();
	}

	if (newParent != parent())
		setParent(newParent);
}


