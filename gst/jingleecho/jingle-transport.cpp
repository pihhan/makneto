
#include "jingle-transport.h"


/*
 *
 * Jingle Transport
 *
 */

void JingleTransport::addCandidate(const JingleCandidate &c)
{
 	candidates.push_back(c);
}

#ifdef GLOOX
using namespace gloox;

void JingleTransport::parse(const Tag *tag)
{
	if (!tag || tag->name() != "transport")
		return;
	m_xmlns = tag->findAttribute("xmlns");
        m_pwd = tag->findAttribute("pwd");
        m_ufrag = tag->findAttribute("ufrag");
	Tag::TagList candidates = tag->findChildren("candidate");
	for (Tag::TagList::iterator it=candidates.begin(); it!=candidates.end(); ++it) {
		JingleUdpCandidate c;
		c.parse(*it);
		addCandidate(c);
	}
}

/** @brief Create transport XML subtree */
Tag * JingleTransport::tag() const
{
    Tag *t = new Tag("transport", "xmlns", m_xmlns);
    if (!m_pwd.empty())
        t->addAttribute("pwd", m_pwd);
    if (!m_ufrag.empty())
        t->addAttribute("ufrag", m_ufrag);
    for (CandidateList::const_iterator it = candidates.begin(); it!=candidates.end(); it++) {
        t->addChild(it->tag());
    }
    return t;
}

#else

QDomElement JingleTransport::tag(QDomDocument &doc) const
{
    QDomElement t = doc.createElement("transport");
    t.setAttribute("xmlns", QString::fromStdString(m_xmlns));
    if (!m_pwd.empty())
        t.setAttribute("pwd", QString::fromStdString(m_pwd));
    if (!m_ufrag.empty())
        t.setAttribute("ufrag", QString::fromStdString(m_ufrag));
    for (CandidateList::const_iterator it = candidates.begin(); it!=candidates.end(); it++) {
        t.appendChild(it->tag(doc));
    }
    return t;
}

void JingleTransport::parse(const QDomElement &tag)
{
    if (tag.tagName() != "transport")
        return;
    m_xmlns = tag.attribute("xmlns").toStdString();
    m_pwd = tag.attribute("pwd").toStdString();
    m_ufrag = tag.attribute("ufrag").toStdString();

    QDomElement child = tag.firstChildElement("candidate");
    while (!child.isNull()) {
        JingleUdpCandidate c;
        c.parse(child);
        addCandidate(c);

        child = child.nextSiblingElement("candidate");
    }
}

#endif

std::string JingleTransport::xmlns()
{
    return m_xmlns;
}

void JingleTransport::setXmlns(const std::string &ns)
{
    m_xmlns = ns;
}

/** @brief Set reachable type for all candidates contained. */
void JingleTransport::setCandidatesReachable(JingleCandidate::ReachableType r)
{
    for (CandidateList::iterator it=candidates.begin(); it!=candidates.end(); it++) {
        it->reachable = r;
    }
}


