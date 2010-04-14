#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QColor>
#include <QDebug>

#include "contactlist.h"
#include "contactlistitem.h"
#include "contactlistgroup.h"
#include "contactlistmodel.h"
#include "contactlistrootitem.h"
#include "contactlistcontact.h"
#include "contactlistgroupitem.h"

#include "maknetocontactlist.h"

#define COLUMNS 3

// TEMPORARY
#define GROUPBG_COLOR QColor(0x96,0x96,0x96)
#define GROUPFG_COLOR QColor(0xFF,0xFF,0xFF)
#define AWAY_COLOR QColor(0x00,0x4b,0xb4)
#define OFFLINE_COLOR QColor(0x64,0x64,0x64)
#define DND_COLOR QColor(0x7e,0x00,0x00)
#define ONLINE_COLOR QColor(0x00,0x00,0x00)

ContactListModel::ContactListModel(MaknetoContactList* contactList) : contactList_(contactList), showStatus_(true)
{
	connect(contactList_,SIGNAL(dataChanged()),this,SLOT(contactList_changed()));
}

/*! \brief Get text description of selected contact list item. */
QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	ContactListItem* item = static_cast<ContactListItem*>(index.internalPointer());
	ContactListGroup*     group     = 0;
	ContactListGroupItem* groupItem = 0;
	ContactListContact*   contact   = 0;

	if (role == Qt::DisplayRole && index.column() == NameColumn) {
		if ((contact = dynamic_cast<ContactListContact*>(item))) {
			QString txt;
            QString name = contact->name();
            if (name.isEmpty())
                name = QString("<%1>").arg(contact->jid());
			if (showStatus_ && !contact->status().message().isEmpty()) {
				txt = QString("%1 (%2)").arg(name).arg(contact->status().message());
			}
			else
				txt = name;
			return QVariant(txt);
		}
		else if ((group = dynamic_cast<ContactListGroup*>(item))) {
			return QVariant(QString("%1 (%2/%3)").arg(group->name()).arg(group->countOnline()).arg(group->count()));
		}
	}
	else if (role == Qt::EditRole && index.column() == NameColumn) {
		if ((contact = dynamic_cast<ContactListContact*>(item))) {
			return QVariant(contact->name());
		}
		else if ((group = dynamic_cast<ContactListGroup*>(item))) {
			return QVariant(group->name());
		}
	}
	else if (role == Qt::DecorationRole && (index.column() == StatusIconColumn || index.column() == PictureColumn)) {
		if ((contact = dynamic_cast<ContactListContact*>(item))) {
			if (index.column() == PictureColumn)
				return QVariant(contact->picture());
			else if (index.column() == StatusIconColumn)
				return QVariant(contact->statusIcon());
		}
	}
	else if (role == Qt::BackgroundColorRole) {
		if ((group = dynamic_cast<ContactListGroup*>(item))) {
			return qVariantFromValue(GROUPBG_COLOR);
		}
	}
	else if (role == Qt::TextColorRole) {
		if ((contact = dynamic_cast<ContactListContact*>(item))) {
			if(contact->status().type() == ContactListStatus::Away || contact->status().type() == ContactListStatus::XA) {
				return qVariantFromValue(AWAY_COLOR);
			}
			else if (contact->status().type() == ContactListStatus::Offline) {
				return qVariantFromValue(OFFLINE_COLOR);
			}
			else if (contact->status().type() == ContactListStatus::DND) {
				return qVariantFromValue(DND_COLOR);
			}
			else {
				return qVariantFromValue(ONLINE_COLOR);
			}
		}
		else if ((group = dynamic_cast<ContactListGroup*>(item))) {
			return qVariantFromValue(GROUPFG_COLOR);
		}
	}
	else if (role == Qt::ToolTipRole) {
		return QVariant(item->toolTip());
	}
	else if (role == ExpandedRole) {
		if ((groupItem = dynamic_cast<ContactListGroupItem*>(item))) {
			if (contactList_->search().isEmpty()) {
				return QVariant(groupItem->expanded());
			}
			else {
				return QVariant(true);
			}
		}
		else {
			return QVariant(false);
		}
	}
	return QVariant();
}

bool ContactListModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
	if (!index.isValid())
		return false;

	ContactListItem* item = static_cast<ContactListItem*>(index.internalPointer());
	if (role == ContextMenuRole) {
		item->showContextMenu(data.toPoint());
                emit dataChanged(index, index);
	}

	return true;
}

QVariant ContactListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}

QModelIndex ContactListModel::index(int row, int column, const QModelIndex &parent) const
{
	ContactListItem* parentItem = 0;
	if (parent.isValid()) {
		parentItem = static_cast<ContactListItem*>(parent.internalPointer());

        ContactListGroupItem *groupItem = dynamic_cast<ContactListGroupItem *>(parentItem);
        if (groupItem) {
            ContactListItem *item = groupItem->atIndex(row);
            return createIndex(row, column, item);
        } else {
        ContactListContact *contact = dynamic_cast<ContactListContact *>(parentItem);
            if (contact) {
                return QModelIndex(); // TODO: add here resource enumeration
            }
        }
    } else {
        QString groupname = contactList_->allGroupNames().at(row);
        ContactListGroupItem *group = contactList_->getGroup(groupname);
        if (group)
            return createIndex(row, column, group);
        else {
            qWarning() << "Group " << groupname << " does not exist, requested at ContactListModel::index()";
            return QModelIndex();
        }
    }
    return QModelIndex();
#if 0
	ContactListItem* item = parentItem->atIndex(row);
	return (item ? createIndex(row, column, item) : QModelIndex());
#endif
}


QModelIndex ContactListModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	ContactListItem * item = static_cast<ContactListItem*>(index.internalPointer());
	ContactListGroupItem* parent = item->parent();

    if (!parent || parent == contactList_->rootItem())
        return QModelIndex();
    else
        return createIndex(parent->index(), 0, parent);
}


int ContactListModel::rowCount(const QModelIndex &parent) const
{
	ContactListGroupItem* parentItem = 0;
	if (parent.isValid()) {
		ContactListItem* item = static_cast<ContactListItem*>(parent.internalPointer());
		parentItem = dynamic_cast<ContactListGroupItem*>(item);
	}
	else {
        return contactList_->groupCount();
		parentItem = contactList_->rootItem();
	}

	return (parentItem ? parentItem->items() : 0);
}


int ContactListModel::columnCount(const QModelIndex&) const
{
	return COLUMNS;
}

Qt::ItemFlags ContactListModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	if (index.column() == NameColumn)
		f = f | Qt::ItemIsEditable;
	return f;
}


void ContactListModel::contactList_changed()
{
	reset();
}


