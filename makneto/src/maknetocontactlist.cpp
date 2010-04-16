/*
 * maknetocontactlist.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include <xmpp_jid.h>
#include <xmpp_status.h>

#include "maknetocontactlist.h"
#include "maknetocontact.h"
#include "contactlist/contactlistgroupitem.h"
#include "contactlist/contactlistrootitem.h"
#include "contactlist/contactlistgroupedcontact.h"


#include "makneto.h"

#include <QtGui/QMenu>
#include <QtGui/QActionGroup>
#include <QtGui/QAction>
#include <QDebug>

#include <iostream>

#include <klocalizedstring.h>

MaknetoContactList::MaknetoContactList(Makneto *makneto) : ContactList(), m_makneto(makneto) 
{ 
  m_contactNewSessionActions = new QActionGroup(this);
  m_contactDetailsActions = new QActionGroup(this);
	connect(m_contactNewSessionActions, SIGNAL(triggered(QAction *)), makneto, SLOT(contactNewSession(QAction *)));
  connect(m_contactDetailsActions, SIGNAL(triggered(QAction *)), makneto, SLOT(contactDetails(QAction *)));
}

/** @brief Create new group item in roster. */
MaknetoGroup *MaknetoContactList::createGroup(const QString &name)
{
    MaknetoGroup *group = new MaknetoGroup(name, rootItem());
    qDebug() << "Created roster group " << name;
    return group;
}

/** @brief Create basic contact popup menu. */
QMenu * MaknetoContactList::createContactMenu(const QString &jid)
{
	QMenu *contactMenu = new QMenu(NULL);
	// create contact menu
  QAction *newSession = new QAction(i18n("New &session"), m_contactNewSessionActions);
	newSession->setData(QVariant(jid));
  
  QAction *contactDetails = new QAction(i18n("&Contact details"), m_contactDetailsActions);
  contactDetails->setData(QVariant(jid));

	//connect(contactDetails, SIGNAL(triggered()), this, SLOT(contactDetails()));

  contactMenu->addAction(newSession);
  contactMenu->addAction(contactDetails);
  return contactMenu;
}

/** @brief Add new contact to roster.
    @param name Nickname of contact added, that user choose for him.
    @param jid JabberId to add.
    @param group Group that contact will be member of. 
*/
void MaknetoContactList::addContact(const QString& name, const QString& jid, const QString& group = QString())
{
	ContactListGroupItem *groupItem = rootItem();
    MaknetoGroup *mg = dynamic_cast<MaknetoGroup *>(groupItem);
	QMenu *contactMenu = createContactMenu(jid);
	QString groupCorrected;

	if (group.isEmpty())
		groupCorrected = "Unknown";
	else
		groupCorrected = group;

	// find/create group
	if (!groupCorrected.isEmpty()) 
	{
		MaknetoGroup g(groupCorrected);

		mg = static_cast<MaknetoGroup*>(rootItem()->findFirstItem(&g));

		if (!mg) 
		{
			mg = static_cast<MaknetoGroup*>(invisibleGroup()->findFirstItem(&g));
		}
	
		if (!mg) 
		{
                    mg = createGroup(groupCorrected);
		}
                groupItem = mg;
	}

  MaknetoContact *contact = new MaknetoContact(name, jid, contactRoot(), contactMenu);
  contact->addGroup(mg);
    
  ContactListGroupedContact *proxy = new ContactListGroupedContact(mg, contact);
  addItem(proxy);
  qDebug() << "Added " << jid << " as " << name << " to roster.";
}

/** @brief Add new contact to roster.
    @param name Nickname of contact added, that user choose for him.
    @param jid JabberId to add.
    @param groups List of groups that contact will be member of. 
    @FIXME change rootItem() way properly
*/
void MaknetoContactList::addContact(const QString& name, const QString& jid, const QStringList& groups)
{
	ContactListGroupItem *groupItem = rootItem();
    MaknetoGroup *mg = dynamic_cast<MaknetoGroup *>(groupItem);
	QMenu *contactMenu = createContactMenu(jid);
	QString groupCorrected = groups.at(0);

	if (groupCorrected.isEmpty())
		groupCorrected = "Unknown";

	// find/create group
	if (!groupCorrected.isEmpty()) 
	{
		MaknetoGroup g(groupCorrected);

		mg = static_cast<MaknetoGroup*>(rootItem()->findFirstItem(&g));

		if (!mg) 
		{
			mg = static_cast<MaknetoGroup*>(invisibleGroup()->findFirstItem(&g));
		}
	
		if (!mg) 
		{
                    mg = createGroup(groupCorrected);
                    addItem(mg);
		}
                groupItem = mg;
	}

  MaknetoContact *contact = new MaknetoContact(name, jid, groupItem, contactMenu);
  for (int i=0; i< groups.size(); i++) {
      QString groupname = groups.at(i);
      if (groupname.isEmpty()) {
          qWarning() << "Skipping invalid empty group for " << jid;
          continue;
      }

      MaknetoGroup *group = 
            dynamic_cast<MaknetoGroup *>(getGroup(groupname));
      if (!group) {
          group = createGroup(groupname);
      }
      contact->addGroup(group);
      // add to group only if it is not already there
      MaknetoContact *tmpcontact = group->findContactByJid(jid);
      if (!tmpcontact) 
          group->addItem(contact);
  }

  addItem(contact);
}

void MaknetoContactList::setAvailability(const QString& jid, const QString &resource, const XMPP::Status& status)
{
	std::cout << "setAvailability(" << jid.toUtf8().data() << ")" << std::endl;

	ContactListGroupItem *root = rootItem();

	MaknetoGroup *groupItem = 0;
	MaknetoContact *item = 0;

	for (int i=0; i<root->items(); i++)
	{
		groupItem = static_cast<MaknetoGroup*>(root->atIndex(i));
		item = 0;

		if (groupItem)
		{
			item = groupItem->findContactByJid(jid);

			if (item)
			{

				item->setStatus(resource, status, m_makneto->getFeatureManager());
	                        emitDataChanged(item);
			}
		}
	}

}

Makneto * MaknetoContactList::makneto()
{
    return m_makneto;
}

MaknetoContact * MaknetoContactList::getContact(const QString &jid)
{
    ContactListContact *contact = ContactList::getContact(jid);
    if (!contact) {
        // if not found, try without resource
        XMPP::Jid xjid(jid);
        if (!xjid.resource().isEmpty()) {
            contact = ContactList::getContact(xjid.bare());
        }
    }
    return dynamic_cast<MaknetoContact *>(contact);
}


#include "maknetocontactlist.moc"
