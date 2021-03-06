#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QStringList>

class ContactListGroupItem;
class ContactListRootItem;
class ContactListItemComparator;
class ContactListItem;
class ContactListContact;
class ContactListGroup;

/*! \brief Implementation of graphical contact list. */
class ContactList : public QObject
{
	Q_OBJECT

	friend class ContactListModel;

public:
        typedef QHash<QString, ContactListGroup *> GroupsHash;
        typedef QHash<QString, ContactListContact *> ContactsHash;

	ContactList(QObject* parent = 0);
	bool showOffline() const { return showOffline_; }
	bool showGroups() const { return showGroups_; }
        bool showOnlyAudio() const;
        bool showOnlyVideo() const;
	ContactListRootItem* invisibleGroup();
	ContactListRootItem* rootItem();
    ContactListRootItem* contactRoot();
	const ContactListItemComparator* itemComparator() const;
	const QString& search() const;

    ContactListContact * getContact(const QString &jid);
    ContactListGroupItem * getGroup(const QString &groupname);
    ContactListGroupItem * addGroup(const QString &name);

	void addItem(ContactListItem*);
	void removeItem(ContactListItem*);

    QStringList allGroupNames() const;
    GroupsHash  allGroups() const;
    ContactsHash allContacts() const;
    unsigned int groupCount() const;
    unsigned int contactCount() const;


signals:
	void dataChanged();
	void dataChangedItem(ContactListItem *item);

public slots:
	void setShowOffline(bool);
	void setShowGroups(bool);
        void setShowOnlyAudio(bool);
        void setShowOnlyVideo(bool);
	void setSearch(const QString&);
	void emitDataChanged(); // Not wild about this one
        void emitDataChanged(ContactListItem *item);

protected:
	void updateParents();
	void updateVisibleParents();
	void updateInvisibleParents();

	//ContactListGroupItem* hiddenGroup();
	//ContactListGroupItem* agentsGroup();
	//ContactListGroupItem* conferenceGroup();

private:
	bool showOffline_, showGroups_;
        bool showAudio_;
        bool showVideo_;
	QString search_;
	ContactListItemComparator* itemComparator_;

	ContactListRootItem* rootItem_;
	ContactListRootItem* invisibleGroup_;
	ContactListRootItem* altInvisibleGroup_;
    ContactListRootItem* contactRootGroup_;

    GroupsHash          groups_; ///!< List of all groups in roster
    ContactsHash        contacts_; ///!< List of all contacts in roster

	//ContactListGroupItem* hiddenGroup_;
	//ContactListGroupItem* agentsGroup_;
	//ContactListGroupItem* conferenceGroup_;
};

#endif
