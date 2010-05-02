
#ifdef GLOOX
#include <gloox/tag.h>
#endif

#include "jingle-content.h"

#ifdef GLOOX
using namespace gloox;
#endif

/*
 *
 * JingleContent
 *
 */

/** @brief Create content without any contents */
JingleContent::JingleContent()
    : m_creator(CREATOR_NONE), m_media(MEDIA_NONE), m_senders(SENDERS_UNKNOWN)
{
}

/** @brief Create content from given transport and description parts. 
    @param transport Transport class.
    @param description Description class.
    */
JingleContent::JingleContent(const JingleTransport &transport, const JingleRtpContentDescription &description)
	: m_transport(transport), m_description(description),
          m_creator(CREATOR_NONE), m_media(MEDIA_NONE), m_senders(SENDERS_UNKNOWN)
{
}

#ifdef GLOOX
/** @brief Parse XML subtree of tag <content> to this structure. */
void JingleContent::parse(const Tag *tag)
{
	if (!tag || tag->name() != "content")
		return;
	m_name = tag->findAttribute("name");
        m_xmlns = tag->findAttribute("xmlns");


	std::string creator = tag->findAttribute("creator");
        if (creator == "initiator")
            m_creator = CREATOR_INITIATOR;
        else if (creator == "responder")
            m_creator = CREATOR_RESPONDER;
        else
            m_creator = CREATOR_NONE;

        std::string senders = tag->findAttribute("senders");
        if (senders == "none")
            m_senders = SENDERS_NONE;
        else if(senders == "initiator")
            m_senders = SENDERS_INITIATOR;
        else if (senders == "responder") 
            m_senders = SENDERS_RESPONDER;
        else if (senders == "both")
            m_senders = SENDERS_BOTH;
        else 
            m_senders = SENDERS_UNKNOWN;

	std::string m_disposition = tag->findAttribute("disposition");
	
	Tag *desc = tag->findChild("description");

	m_description = JingleRtpContentDescription();
	m_description.parse(desc);
	Tag *transport = tag->findChild("transport");
	m_transport = JingleTransport();
	m_transport.parse(transport);
}

/** @brief Create xml subtree for content tag. */
Tag * JingleContent::tag() const
{
    Tag *t = new Tag("content");
    if (!m_name.empty())
            t->addAttribute("name", m_name);
    switch (m_media) {
        case MEDIA_AUDIO:
            t->addAttribute("media", "audio"); break;
        case MEDIA_VIDEO:
            t->addAttribute("media", "video"); break;
        case MEDIA_NONE:
            break;
    }

    switch (m_creator) {
        case CREATOR_INITIATOR:
            t->addAttribute("creator", "initiator"); break;
        case CREATOR_RESPONDER:
            t->addAttribute("creator", "responder"); break;
        case CREATOR_NONE:
            break;
    }

    switch (m_senders) {
        case SENDERS_NONE:
            t->addAttribute("senders", "none"); break;
        case SENDERS_INITIATOR:
            t->addAttribute("senders", "initiator"); break;
        case SENDERS_RESPONDER:
            t->addAttribute("senders", "responder"); break;
        case SENDERS_BOTH:
            t->addAttribute("senders", "both"); break;
        case SENDERS_UNKNOWN:
            break;
    }

    Tag *transport = m_transport.tag();
    if (transport)
            t->addChild(transport);
    Tag *description = m_description.tag();
    if (description)
            t->addChild(description);
    return t;
}

#else // IRIS
/** @brief Parse XML subtree of tag <content> to this structure. */
void JingleContent::parse(const QDomElement &tag)
{
	if (tag.tagName() != "content")
		return;
	m_name = tag.attribute("name").toStdString();
        //m_xmlns = tag.attribute("xmlns").toStdString();

	std::string media = tag.attribute("media").toStdString();
        if (media == "audio")
            m_media = MEDIA_AUDIO;
        else if (media == "video")
            m_media = MEDIA_VIDEO;
        else
            m_media = MEDIA_NONE;

	std::string creator = tag.attribute("creator").toStdString();
        if (creator == "initiator")
            m_creator = CREATOR_INITIATOR;
        else if (creator == "responder")
            m_creator = CREATOR_RESPONDER;
        else
            m_creator = CREATOR_NONE;

        std::string senders = tag.attribute("senders").toStdString();
        if (senders == "none")
            m_senders = SENDERS_NONE;
        else if(senders == "initiator")
            m_senders = SENDERS_INITIATOR;
        else if (senders == "responder") 
            m_senders = SENDERS_RESPONDER;
        else if (senders == "both")
            m_senders = SENDERS_BOTH;
        else 
            m_senders = SENDERS_UNKNOWN;

	std::string m_disposition = tag.attribute("disposition").toStdString();
	
	QDomElement desc = tag.firstChildElement("description");

	m_description = JingleRtpContentDescription();
	m_description.parse(desc);
	QDomElement transport = tag.firstChildElement("transport");
	m_transport = JingleTransport();
	m_transport.parse(transport);
}

/** @brief Create xml subtree for content tag. */
QDomElement JingleContent::tag(QDomDocument &doc) const
{
    QDomElement t = doc.createElement("content");
    //t.setAttribute("xmlns", QString::fromStdString(m_xmlns));
    if (!m_name.empty())
            t.setAttribute("name", QString::fromStdString(m_name));
    switch (m_media) {
        case MEDIA_AUDIO:
            t.setAttribute("media", "audio"); break;
        case MEDIA_VIDEO:
            t.setAttribute("media", "video"); break;
        case MEDIA_NONE:
        case MEDIA_AUDIOVIDEO:
            break;
    }

    switch (m_creator) {
        case CREATOR_INITIATOR:
            t.setAttribute("creator", "initiator"); break;
        case CREATOR_RESPONDER:
            t.setAttribute("creator", "responder"); break;
        case CREATOR_NONE:
            break;
    }

    switch (m_senders) {
        case SENDERS_NONE:
            t.setAttribute("senders", "none"); break;
        case SENDERS_INITIATOR:
            t.setAttribute("senders", "initiator"); break;
        case SENDERS_RESPONDER:
            t.setAttribute("senders", "responder"); break;
        case SENDERS_BOTH:
            t.setAttribute("senders", "both"); break;
        case SENDERS_UNKNOWN:
            break;
    }

    QDomElement transport = m_transport.tag(doc);
    if (!transport.isNull())
            t.appendChild(transport);
    QDomElement description = m_description.tag(doc);
    if (!description.isNull())
            t.appendChild(description);
    return t;
}
#endif // GLOOX else IRIS

JingleRtpContentDescription JingleContent::description() const
{ 
    return m_description;
}

JingleTransport JingleContent::transport() const
{ 
    return m_transport;
}

std::string JingleContent::name() const
{
    return m_name;
}

JingleContent::Creator JingleContent::creator() const
{
    return m_creator;
}

MediaType   JingleContent::media() const
{
    return m_description.type();
}

std::string JingleContent::disposition() const
{
    return m_disposition;
}

JingleContent::Senders JingleContent::senders() const 
{
    return m_senders;
}

JingleContent::State    JingleContent::state() const
{
    return m_state;
}

void JingleContent::setName(const std::string &name)
{
    m_name = name;
}

void JingleContent::setCreator(Creator creator)
{
    m_creator = creator;
}

void JingleContent::setMedia(MediaType media)
{
    m_description.setType(media);
}

void JingleContent::setDisposition(const std::string &disposition)
{
    m_disposition = disposition;
}

void JingleContent::setSenders(JingleContent::Senders s)
{
    m_senders = s;
}

void JingleContent::setTransport(JingleTransport &transport)
{
    m_transport = transport;
}

void JingleContent::setDescription(JingleRtpContentDescription &desc)
{
    m_description = desc;
}

void JingleContent::setState(State s)
{
    m_state = s;
}
