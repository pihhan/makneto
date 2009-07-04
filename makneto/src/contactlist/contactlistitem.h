#ifndef CONTACTLISTITEM_H
#define CONTACTLISTITEM_H

#include <QString>
#include <QPoint>
#include <QList>

class ContactList;
class ContactListGroupItem;
class QMenu;

/*! \brief Unsecified type of item in contact list. */
class ContactListItem 
{
public:
	ContactListItem(ContactListGroupItem* parent);
	virtual ~ContactListItem() {};

        /*! \brief Get contact list instance. */
	virtual ContactList* contactList() const;
	virtual ContactListGroupItem* parent() const;
	virtual ContactListGroupItem* defaultParent() const;
	virtual int index();
	virtual bool isVisible() const;
	virtual bool equals(ContactListItem*) const;
	virtual QString toolTip() const;
	virtual void updateParent();

	// Grouping functions
	virtual ContactListItem* findFirstItem(ContactListItem* other);
	virtual int count() const;
	virtual int countOnline() const;
	virtual void updateParents();
	//virtual QList<ContactListItem*> visibleItems();
	//virtual QList<ContactListItem*> invisibleItems();

	virtual void setParent(ContactListGroupItem* parent);
	virtual void showContextMenu(const QPoint&);


private:
	ContactListGroupItem* parent_;
	ContactListGroupItem* defaultParent_;
};

#endif
