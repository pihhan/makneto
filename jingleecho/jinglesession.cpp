
#include <cstdlib>

#include "jinglesession.h"



using namespace gloox;

void JingleSession::parse(const Stanza *stanza)
{
	Tag *jingle = stanza->findChild("jingle");
	if (!jingle)
		return;
	m_sid = jingle->findAttribute("sid");
	m_initiator = jingle->findAttribute("initiator");
	std::string action = jingle->findAttribute("action");
	Tag::TagList contents = jingke->findChildren("content");
	
	for (Tag::TagList::iterator it=contents.begin(); it!=contents.end(); it++) {
		JingleContent *content = new JingleContent();
		content->parse(*it);
	}
}

void JingleContent::parse(const Tag *tag)
{
	if (!tag || tag->name() != "content")
		return;
	m_creator = tag->findAttribute("creator");
	m_name = tag->findAttribute("name");
	m_media = tag->findAttribute("media");
	
	Tag *desc = tag->findChild("description");

	m_description = JingleRtpContentDescription();
	m_description.parse(desc);
	Tag *transport = tag->findChild("transport");
	m_transport = JingleTransport();
	m_transport.parse(transport);
}

void JingleTransport::parse(const Tag *tag)
{
	if (!tag || tag->name() != "transport")
		return;
	m_xmlns = tag->findAttribute("xmlns");
	Tag::TagList candidates = tag->findChildren("candidate");
	for (Tag::TagList::iterator it=candidates.begin(); it!=candidates.end(); ++it) {
		JingleUdpCandidate *c = new JingleUdpCandidate();
		c->parse(*it);
		addCandidate(c);
	}
}


void JingleTransport::addCandidate(JingleCandidate *c)
{
 	candidates.push_back(c);
}


void JingleCandidate::parse(const Tag *tag)
{
	if (!tag || tag->name() != "candidate")
		return;
	component = atoi(tag->findAttribute("component").c_str());
	ip 		= tag->findAttribute("ip");
	port 	= atoi(tag->findAttribute("port").c_str());
	id		= tag->findAttribute("id");
	generation = atoi(tag->findAttribute("generation").c_str());
}

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


void JingleRtpContentDescription::parse(const Tag *tag)
{
	if (!tag || tag->name() != "description")
		return;
	m_xmlns = tag->findAttribute("xmlns");
	m_media = tag->findAttribute("media");
	
	Tag::TagList payloads = tag->findChildren("payload");
	for (Tag::TagList::iterator it=payloads.begin(); it!=payloads.end(); ++it) {
		JingleRtpPayload payload;
		payload.parse(*it);
		addPayload(payload);
	}
}

void JingleRtpPayload::parse(const Tag *tag)
{
	if(!tag || tag->name() != "payload")
		return;
	id = atoi( tag->findAttribute("id").c_str() );
	channels = atoi( tag->findAttribute("channels");
	name = tag->findAttribute("name");
	clockrate = atoi(tag->findAttribute("clockrate"));
	ptime = atoi( tag->findAttribute("ptime").c_str());
	maxptime = atoi( tag->findAttribute("maxptime").c_str());
}
