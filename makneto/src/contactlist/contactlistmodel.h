#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>

#include "contactlistitem.h"

class ContactList;
class MaknetoContactList;


class ContactListModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	enum {
		ExpandedRole = Qt::UserRole + 0,
		ContextMenuRole = Qt::UserRole + 1
	};
	enum {
		StatusIconColumn = 0,
		NameColumn = 1,
		PictureColumn = 2
	};

    enum ContactListFilter {
        FILTER_ONLINE = (1 << 0),
        FILTER_WHITEBOARD = (1 << 1),
        FILTER_AUDIO = (1 << 2),
        FILTER_VIDEO = (1 << 3),
    };

	ContactListModel(MaknetoContactList* contactList);

	// Reimplemented from QAbstratItemModel
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent) const;
	virtual int columnCount(const QModelIndex &parent) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual bool setData(const QModelIndex&, const QVariant&, int role);
    
    bool passFilter(const ContactListItem *item) const;
    QList<ContactListItem *>    getFilteredItems(const QModelIndex &parent) const;

    void setFilter(int filterFlags);
    int filter() const;

public slots:
    void itemChanged(ContactListItem *item);
protected slots:
	void contactList_changed();

private:
	MaknetoContactList* contactList_;
	bool showStatus_;
    int  filter_;
};

#endif
