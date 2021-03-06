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
            case ContactListStatus::Online:
                        return KIcon("maknetoonline");
            case ContactListStatus::Away:
                        return KIcon("maknetoaway");
            case ContactListStatus::XA:
                        return KIcon("maknetoxa");
            case ContactListStatus::DND:
                        return KIcon("maknetodnd");
            case ContactListStatus::Invisible:
                        return KIcon("maknetoinvisible");
            case ContactListStatus::FFC:
                        return KIcon("maknetoffc");
            case ContactListStatus::Error:
                        return KIcon("maknetoerror");
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

bool MaknetoContactResource::supportsVideo() const
{
    return supportsFeature("urn:xmpp:jingle:video");
}

bool MaknetoContactResource::supportsAudio() const
{
    return supportsFeature("urn:xmpp:jingle:audio");
}

bool MaknetoContactResource::supportsWhiteboard() const
{
    return supportsFeature("http://jabber.org/protocol/svgwb");
}



void MaknetoContactResource::updateParent() 
{
}

