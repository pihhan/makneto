/*
 * maknetocontact.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "maknetocontact.h"
#include "contactlist/contactlistgroupitem.h"
#include "contactlist/contactlistgroupedcontact.h"
#include "contactlist/status.h"
#include "maknetocontactlist.h"

#include <xmpp_status.h>
#include <xmpp_jid.h>

#include <QtGui/QMenu>
#include <QtDebug>
#include <KIcon>

#include <iostream>


MaknetoContact::MaknetoContact(const QString& name, const QString& jid, 
        ContactListGroupItem* parent, QMenu *contactMenu
    ) :
      ContactListContact(parent), m_name(name), m_jid(jid), 
      m_status(ContactListStatus::Offline, "Offline"), 
      m_contactMenu(contactMenu)
{

}

MaknetoContact::~MaknetoContact()
{

}

void MaknetoContact::showContextMenu(const QPoint &where)
{
	m_contactMenu->exec(where);
}

QIcon MaknetoContact::statusIcon() const
{
    MaknetoContactResource r;
    r = bestResourceR();
    if (!r.isNull())
        return r.statusIcon();
    else
        return QIcon();
}


ContactListStatus MaknetoContact::status() const 
{ 
    MaknetoContactResource resource;
    resource = bestResourceR();
    if (!resource.isNull()) {
        return resource.status();
    } else {
        return ContactListStatus(ContactListStatus::Offline); 
    }
}

void MaknetoContact::setStatus(const XMPP::Status &status)
{
    MaknetoContactResource *resource;
    resource = bestResource();
    if (resource != NULL) {
        resource->setStatus(status);
        emit contactChanged();
    }
}

/** \brief Create new resource based on incoming presence. */
MaknetoContactResource * MaknetoContact::createResource(const QString &resource, const XMPP::Status &status)
{
    MaknetoContactResource *r;
    r = new MaknetoContactResource(this, status, resource);
    m_resources.insert(resource, r);
#ifdef CAPS_DEBUG
    if (!status.capsVersion().isEmpty()) {
        qDebug() << "Contact has caps string " << status.capsVersion() << " and node " << status.capsNode();
    }
#endif
    if (m_contactMenu) {
        QAction *action= new QAction(resource, m_contactMenu);
        action->setData(QVariant(r->jid()));
        MaknetoContactList *list = dynamic_cast<MaknetoContactList *>(contactList());
        if (list) {
            qDebug("List is present");

        }

        m_contactMenu->addAction((action));
        QList<QAction *> actions = m_contactMenu->actions();
        for (QList<QAction *>::iterator it= actions.begin();
            it != actions.end(); it++) {
#ifdef CAPS_DEBUG
            qDebug() << "Menu Action: " << (*it)->text();
#endif
        }
    }
    return r;
}

/** \brief Remove resource from contact. */
void MaknetoContact::removeResource(const QString &resource)
{
    MaknetoContactResource *r = m_resources.take(resource);
    delete r;
    qDebug() << "Removing resource " << resource << " for contact "
        << jid();

    QList<QAction *> actions = m_contactMenu->actions();
    for (QList<QAction *>::iterator it= actions.begin();
        it != actions.end(); it++) {
        if ((*it)->text() == resource) {
            m_contactMenu->removeAction((*it));
            qDebug() << "Removing Menu Action: " << (*it)->text();
        }
    }
}

void MaknetoContact::setStatus(const QString &resource, const XMPP::Status &status, FeatureListManager *flm)
{
    MaknetoContactResource *r;
    r = this->resource(resource);
    if (r && !r->isNull()) {
        r->setStatus(status);
        if (status.type() == XMPP::Status::Offline) {
            removeResource(resource);
            r = NULL;
        }
    } else {
        // takove resource neexistuje, pokud neni offline, musim ho udelat
        if (status.type() != XMPP::Status::Offline) {
            r = createResource(resource, status);
        }
    }

    if (r != NULL) {
            // TODO: \todo Iris nepodporuje capsHash, nebude aktualni capabilities
            QString node = status.capsNode();
            QString ver = status.capsVersion();
            QString hash;
            QString ext = status.capsExt();
            FeatureList *origfl = r->features();
            bool changed = (!origfl || origfl->hasFeaturesChanged(node, ver, hash, ext)); 
            if (flm != NULL && changed && !ver.isEmpty()) {
                qDebug() << "Features of " << m_jid << "/" << resource << " changed";
                FeatureList *fl;
                if (flm->isKnown(node,ver,hash,ext)) {
                    fl = flm->getFeatures(node,ver,hash,ext);
                    if (!fl)
                        qWarning("Features should be known, but returned NULL");
                    r->setFeatures( fl, true );
                    qDebug() << "Features found in local cache"; 
                } else {
                    XMPP::Jid jid1(m_jid);
                    XMPP::Jid jid2(jid1.node(), jid1.domain(), resource);
                    flm->requestFeatureUpdate(jid2, node,ver,hash,ext);
                    qDebug() << "Features not known and requested from " << jid2.full();
                }
                
            }
    }
    emit contactChanged();
}

QString MaknetoContact::resource() const
{
    QString r;
//            MaknetoContactResource *br = bestResource();
//            if (br)
//                r = br->resource();
    return r;
}

int MaknetoContact::priority() const
{
    int r = 0;
//            MaknetoContactResource *br = bestResource();
//            if (br)
//                r = br->priority();
    return r;
}

bool MaknetoContact::supportsFeature(const QString &feature) const
{
    bool supported = false;
    for (ResourcesHash::const_iterator it=m_resources.begin(); it != m_resources.end(); it++) {
            supported = supported || (*it)->supportsFeature(feature);
    }
    return supported;
}

bool MaknetoContact::supportsVideo() const
{
    return supportsFeature("urn:xmpp:jingle:video");
}

bool MaknetoContact::supportsAudio() const
{
    return supportsFeature("urn:xmpp:jingle:audio");
}

bool MaknetoContact::supportsWhiteboard() const
{
    return supportsFeature("http://jabber.org/protocol/svgwb");
}

bool MaknetoContact::isOnline() const
{
    return (resourcesNumber() > 0);
}

void MaknetoContact::updateParent()
{
    // noop
}

//
// MaknetoGroup
//
//

MaknetoGroup::MaknetoGroup(const QString& name, ContactListGroupItem* parent)
    : ContactListGroup(parent), m_name(name) 
{
}


//MaknetoContact* MaknetoGroup::findContactByJid(const QString& jid)
ContactListGroupedContact * MaknetoGroup::findContactByJid(const QString& jid)
{
	ContactListGroupedContact *item = 0;
	bool found = false;
	int i = 0;

	while(!found && i<items())
	{
		item = static_cast<ContactListGroupedContact *>(atIndex(i));
		
		if (item && item->jid() == jid)
			found = true;

		i++;
	}

	if (found)
		return item;
	else
		return 0;
}


