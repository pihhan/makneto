/***
 * File to represent one resource of contact for contactlist.
 * @author Petr Mensik <xmensi06@stud.fit.vutbr.cz>
 */

#include "maknetocontact.h"
#include "contactlist/contactlistgroupitem.h"
#include "contactlist/status.h"
#include "maknetocontactlist.h"

#include <xmpp_status.h>
#include <xmpp_jid.h>

#include <QtGui/QMenu>
#include <QtDebug>
#include <KIcon>

#include <iostream>




MaknetoContactResource::MaknetoContactResource()
    : ContactListContact(NULL),m_bare(NULL),
      m_status(ContactListStatus::Offline),
      m_contactMenu(NULL), 
      m_priority(0), m_features(0), m_feature_shared(false), m_null(true)
{}

MaknetoContactResource::MaknetoContactResource(MaknetoContact *bare, const QString &resource, int priority)
    : ContactListContact(NULL),m_bare(bare), 
      m_status(ContactListStatus::Offline),
      m_contactMenu(NULL), 
      m_resource(resource),m_priority(priority), m_features(0), 
      m_feature_shared(false), m_null(false)
{}



/*! \brief Create resource from first incoming presence. */
MaknetoContactResource::MaknetoContactResource(MaknetoContact *bare, const XMPP::Status &status, const QString &resource)
    : ContactListContact(NULL), m_bare(bare),
        m_status(ContactListStatus::Offline),m_resource(0), m_features(0),
        m_feature_shared(false), m_null(false)
{
    m_resource = resource;
    setStatus(status);
    // TODO: caps hash checking a inicializace features.
}

MaknetoContactResource MaknetoContact::bestResourceR() const
{
    MaknetoContactResource p;
    ResourcesHash::const_iterator it;
    bool notused = true;
    for (it= m_resources.begin(); it != m_resources.end(); it++) {
        if (notused || p < *(*it)) {
            p = *(*it);
            notused=false;
        }
    }
    return p;
}

MaknetoContactResource * MaknetoContact::bestResource() 
{
    MaknetoContactResource *p = NULL;
    ResourcesHash::iterator it;
    for (it= m_resources.begin(); it != m_resources.end(); it++) {
        if (!p || *p < *(*it)) {
            p = (*it);
        }
    }
    return p;
}

/*! \brief Return full jid of contact, also with resource. */
QString MaknetoContactResource::jid() const
{
    XMPP::Jid jid1(m_bare->jid());
    XMPP::Jid jid2(jid1.node(), jid1.domain(), m_resource);
    return jid2.full();
}

void MaknetoContactResource::setFeatures(FeatureList *fl, bool shared)
{
    m_feature_shared = shared;
    m_features = fl;
}

void MaknetoContactResource::setStatus(const XMPP::Status &status)
{
	switch (status.type())
	{
		case XMPP::Status::Offline:
			m_status = ContactListStatus(ContactListStatus::Offline, "Offline");
			break;

		case XMPP::Status::Online:
			m_status = ContactListStatus(ContactListStatus::Online, "Online");
			break;

		case XMPP::Status::Away:
			m_status = ContactListStatus(ContactListStatus::Away, "Away");
			break;

		case XMPP::Status::XA:
			m_status = ContactListStatus(ContactListStatus::XA, "XA");
			break;

		case XMPP::Status::DND:
			m_status = ContactListStatus(ContactListStatus::DND, "DND");
			break;

		case XMPP::Status::Invisible:
			m_status = ContactListStatus(ContactListStatus::Invisible, "Invisible");
			break;

		case XMPP::Status::FFC:
			m_status = ContactListStatus(ContactListStatus::FFC, "Free for chat");
			break;
	}
}

QIcon MaknetoContactResource::statusIcon() const
{
	switch (status().type())
	{
    case ContactListStatus::Offline:
			return KIcon("maknetooffline");
			break;

    case ContactListStatus::Online:
			return KIcon("maknetoonline");
			break;

    case ContactListStatus::Away:
			return KIcon("maknetoaway");
			break;

    case ContactListStatus::XA:
			return KIcon("maknetoxa");
			break;

    case ContactListStatus::DND:
			return KIcon("maknetodnd");
			break;

    case ContactListStatus::Invisible:
			return KIcon("maknetoinvisible");
			break;

    case ContactListStatus::FFC:
			return KIcon("maknetoffc");
			break;
	}

	return QIcon();
}

bool MaknetoContactResource::supportsFeature(const QString &feature) const
{
    if (m_features) {
            return m_features->supported(feature);
    } else 
        return false;
}

