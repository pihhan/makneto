
#include <cstdlib>

#ifdef GLOOX
#include <gloox/tag.h>
#else
#include <QDomElement>
#endif

#include "jingle-description.h"
#include "jinglesession.h"

#ifdef GLOOX
using namespace gloox;
#endif

/*
 *
 * JingleRtpContentDescription
 *
 */

JingleRtpContentDescription::JingleRtpContentDescription()
    : m_xmlns(XMLNS_JINGLE_RTP), m_type(MEDIA_NONE)
{
}

#ifdef GLOOX
/** @brief Fill class from given XML subtree, tag must be named <description> */
void JingleRtpContentDescription::parse(const Tag *tag)
{
	if (!tag || tag->name() != "description")
		return;
	m_xmlns = tag->findAttribute("xmlns");
	m_media = tag->findAttribute("media");
        if (m_media == "audio") {
            m_type = MEDIA_AUDIO;
        } else if (m_media == "video") {
            m_type = MEDIA_VIDEO;
        } else {
            m_type = MEDIA_NONE;
        }
	
	Tag::TagList payloads = tag->findChildren("payload-type");
	for (Tag::TagList::iterator it=payloads.begin(); it!=payloads.end(); ++it) {
		JingleRtpPayload payload(*it);
		addPayload(payload);
	}
}

/** @brief Return XML subtree with description tag. */
Tag *JingleRtpContentDescription::tag() const
{
	Tag *t = new Tag("description");
	t->addAttribute("xmlns", m_xmlns);
        switch (m_type) {
            case MEDIA_VIDEO:
                t->addAttribute("media", "video");
                break;
            case MEDIA_AUDIO:
                t->addAttribute("media", "audio");
                break;
            case MEDIA_NONE:
	        if (!m_media.empty())
		     t->addAttribute("media", m_media);
                break;
        }
	for (PayloadList::const_iterator it=payloads.begin(); it!=payloads.end(); ++it) {
		t->addChild(it->tag());
	}
	return t;
}

#else // GLOOX_API

void JingleRtpContentDescription::parse(const QDomElement &tag)
{
    if (tag.tagName() != "description")
        return;
    m_xmlns = tag.attribute("xmlns").toStdString();
    m_media = tag.attribute("media").toStdString();
    if (m_media == "audio") {
        m_type = MEDIA_AUDIO;
    } else if (m_media == "video") {
        m_type = MEDIA_VIDEO;
    } else {
        m_type = MEDIA_NONE;
    }
    QDomNodeList payloads = tag.elementsByTagName("payload-type");
    for (unsigned int i = 0; i < payloads.length(); i++) {
        QDomElement n = payloads.at(i).toElement();
        JingleRtpPayload payload(n);
        addPayload(payload);
    }
}

QDomElement JingleRtpContentDescription::tag(QDomDocument &doc) const
{
    QDomElement t = doc.createElement("description");
    t.setAttribute("xmlns", QString::fromStdString(m_xmlns));
    switch (m_type) {
        case MEDIA_VIDEO:
            t.setAttribute("media", "video");
            break;
        case MEDIA_AUDIO:
            t.setAttribute("media", "audio");
            break;
        case MEDIA_NONE:
        case MEDIA_AUDIOVIDEO:
            if (!m_media.empty()) 
                t.setAttribute("media", QString::fromStdString(m_media));
            break;
    }

    for (PayloadList::const_iterator it=payloads.begin(); it!=payloads.end(); ++it) {
            t.appendChild(it->tag(doc));
    }
    return t;
}
#endif // GLOOX

std::string JingleRtpContentDescription::media() const
{ return m_media; }

std::string JingleRtpContentDescription::xmlns() const
{ return m_xmlns; }

MediaType   JingleRtpContentDescription::type() const
{ return m_type; }

void JingleRtpContentDescription::setType(MediaType type)
{
    m_type = type;
}

void JingleRtpContentDescription::addPayload(const JingleRtpPayload &payload) 
{ 
    payloads.push_back(payload); 
}

void JingleRtpContentDescription::clearPayload()
{
    payloads.clear();
}

int JingleRtpContentDescription::countPayload() const
{
    return payloads.size();
}

/*
 *
 * JingleRtpPayload
 *
 */

JingleRtpPayload::JingleRtpPayload(unsigned char id, const std::string &name, unsigned int clockrate, int channels)
	: id(id), name(name), clockrate(clockrate), channels(channels), maxptime(0), ptime(0)
{
}

#ifdef GLOOX
JingleRtpPayload::JingleRtpPayload(const Tag *tag)
    : id(0), clockrate(0), channels(1), maxptime(0), ptime(0)
{
    parse(tag);
}
	
void JingleRtpPayload::parse(const Tag *tag)
{
	if(!tag || tag->name() != "payload-type")
		return;
	id = atoi( tag->findAttribute("id").c_str() );
	channels = atoi( tag->findAttribute("channels").c_str());
	name = tag->findAttribute("name");
	clockrate = atoi(tag->findAttribute("clockrate").c_str());
	ptime = atoi( tag->findAttribute("ptime").c_str());
	maxptime = atoi( tag->findAttribute("maxptime").c_str());

        Tag::TagList tagp = findChildren("parameter");
        for (Tag::TagList::iterator i=tagp.begin(); i!=tagp.end(); it++) {
            add(PayloadParameter(
                (*i)->findAttribute("name"), 
                (*i)->findAttribute("value"))
            );
        }
}

Tag * JingleRtpPayload::tag() const
{
	Tag *t = new Tag("payload-type");
	if (!t) return NULL;
	t->addAttribute("id", id);
	t->addAttribute("name", name);
	t->addAttribute("clockrate", (long) clockrate);
	if (channels)
		t->addAttribute("channels", channels);
	if (ptime)
		t->addAttribute("ptime", (long) ptime);
	if (maxptime)
		t->addAttribute("maxptime", (long) maxptime);

        for (ParameterList::iterator it=parameters.begin(); 
            it!=parameters.end(); it++)  
        {
            Tag *param = new Tag(t, "parameter");
            param->addAttribute("name", it->name());
            param->addAttribute("value", it->stringValue());
        }
	return t;
}

#else // GLOOX

JingleRtpPayload::JingleRtpPayload(const QDomElement &tag)
{
    parse(tag);
}

void JingleRtpPayload::parse(const QDomElement &tag)
{
    if (tag.tagName() != "payload-type")
        return;
    id = tag.attribute("id").toInt();
    channels = tag.attribute("channels").toInt();
    name = tag.attribute("name").toStdString();
    clockrate = tag.attribute("clockrate").toInt();
    ptime = tag.attribute("ptime").toInt();
    maxptime = tag.attribute("maxptime").toInt();

    QDomElement parameter = tag.firstChildElement("parameter");
    while (!parameter.isNull()) {
        std::string name = parameter.attribute("name").toStdString();
        std::string value = parameter.attribute("value").toStdString();
        add(PayloadParameter(name, value));
        parameter = parameter.nextSiblingElement("parameter");
    }
}

/** @brief Convert payload to QDomElement tag. */
QDomElement JingleRtpPayload::tag(QDomDocument &doc) const
{
    QDomElement t = doc.createElement("payload-type");
    t.setAttribute("id", id);
    t.setAttribute("name", QString::fromStdString(name));
    t.setAttribute("clockrate", clockrate);
    t.setAttribute("ptime", ptime);
    t.setAttribute("maxptime", maxptime);

    for (ParameterList::const_iterator it=parameters.begin();
        it!=parameters.end(); it++) 
    {
        QDomElement p = doc.createElement("parameter");
        p.setAttribute("name", QString::fromStdString(it->name()));
        p.setAttribute("value", QString::fromStdString(it->stringValue()));
        t.appendChild(p);
    }
    return t;
}

#endif

/** @brief Add new parameter to payload. */
void JingleRtpPayload::add(const PayloadParameter &p)
{
    parameters.push_back(p);
}




