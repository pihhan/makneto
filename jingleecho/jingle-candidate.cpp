
#include <cstdlib>
#include "jingle-candidate.h"

#ifdef GLOOX
using namespace gloox;
#endif

/*
 *
 * JingleCandidate
 *
 */

JingleCandidate::JingleCandidate()
    : component(0), port(0), generation(0), natType(NAT_NONE), 
      candidateType(TYPE_HOST), reachable(REACHABLE_UNKNOWN)
{
}

std::string JingleCandidate::xmlns() const
{
    return XMLNS_JINGLE_RAWUDP;
}

void JingleCandidate::increaseGeneration() 
{
    ++generation;
}

#ifdef GLOOX
void JingleCandidate::parse(const Tag *tag)
{
	if (!tag || tag->name() != "candidate")
		return;
	component = atoi(tag->findAttribute("component").c_str());
	ip 		= tag->findAttribute("ip");
	port 	= atoi(tag->findAttribute("port").c_str());
	id		= tag->findAttribute("id");
	generation = atoi(tag->findAttribute("generation").c_str());
}

Tag* JingleCandidate::tag() const
{
	Tag *t = new Tag("candidate");
	if (!t) return NULL;
	t->addAttribute("component", component);
	t->addAttribute("ip", ip);
	t->addAttribute("port", (long) port);
	t->addAttribute("id", id);
	t->addAttribute("generation", generation);
	return t;
}

#else

QDomElement JingleCandidate::tag(QDomDocument &doc) const
{
    QDomElement t = doc.createElement("candidate");
    t.setAttribute("component", component);
    t.setAttribute("ip", QString::fromStdString(ip));
    t.setAttribute("port", port);
    t.setAttribute("id", QString::fromStdString(id));
    t.setAttribute("generation", generation);
    return t;
}

void JingleCandidate::parse(const QDomElement &tag)
{
    if (tag.tagName() != "candidate")
        return;
    component = tag.attribute("component").toInt();
    ip = tag.attribute("ip").toStdString();
    port = tag.attribute("port").toInt();
    id = tag.attribute("id").toStdString();
    generation = tag.attribute("generation").toInt();
}
#endif // GLOOX_API

/*
 *
 * JingleIceCandidate
 *
 */

#ifdef GLOOX
void JingleIceCandidate::parse(const Tag *tag)
{
	if (!tag || tag->name() != "candidate")
		return;
	JingleCandidate::parse(tag);
	foundation	=	atoi(tag->findAttribute("foundation").c_str());
	network		=	atoi(tag->findAttribute("network").c_str());
	priority	=	atoi(tag->findAttribute("priority").c_str());
	protocol	=	tag->findAttribute("protocol");
	type		=	tag->findAttribute("type");
	relAddr		=	tag->findAttribute("relAddr");
	relPort		=	tag->findAttribute("relPort");
}

Tag * JingleIceCandidate::tag() const
{
	Tag *t = JingleCandidate::tag();
	t->addAttribute("foundation", foundation);
	t->addAttribute("network", network);
	t->addAttribute("priority", priority);
	t->addAttribute("protocol", protocol);
	t->addAttribute("type", type);
	if (!relAddr.empty())
		t->addAttribute("relAddr", relAddr);
	if (!relPort.empty())
		t->addAttribute("relPort", relPort);
	return t;
}
#else

QDomElement JingleIceCandidate::tag(QDomDocument &doc) const
{
    QDomElement t = JingleCandidate::tag(doc);
    t.setAttribute("foundation", foundation);
    t.setAttribute("network", network);
    t.setAttribute("priority", priority);
    switch (protocol) {
        case PR_UDP:
            t.setAttribute("protocol", "udp"); break;
        case PR_TCP:
            t.setAttribute("protocol", "tcp"); break;
        case PR_UNSPECIFIED:
            break;
    }
    t.setAttribute("type", QString::fromStdString(type));
    if (!relAddr.empty())
        t.setAttribute("relAddr", QString::fromStdString(relAddr));
    if (relPort != 0)
        t.setAttribute("relPort", relPort);
    return t;
}

void JingleIceCandidate::parse(const QDomElement &tag)
{
    if (tag.tagName() != "candidate")
        return;
    JingleCandidate::parse(tag);
    foundation = tag.attribute("foundation").toInt();
    network = tag.attribute("network").toInt();
    priority = tag.attribute("priority").toInt();
    QString protostr = tag.attribute("protocol");
    if (protostr == "udp")
        protocol = PR_UDP;
    else if (protostr == "tcp")
        protocol = PR_TCP;
    type = tag.attribute("type").toStdString();
    relAddr = tag.attribute("relAddr").toStdString();
    relPort = tag.attribute("relPort").toUInt();
}

#endif //GLOOX_API

JingleIceCandidate::JingleIceCandidate()
    :  foundation(0), protocol(PR_UDP), priority(0)
{
}

JingleIceCandidate::JingleIceCandidate(const JingleCandidate &candidate)
    : JingleCandidate(candidate), foundation(0), protocol(PR_UDP), priority(0)
{
}

JingleCandidate JingleIceCandidate::operator=(const JingleCandidate &c)
{
    JingleCandidate::operator=(c);
    return c;
}

std::string JingleIceCandidate::xmlns() const
{
    return XMLNS_JINGLE_ICE;
}

