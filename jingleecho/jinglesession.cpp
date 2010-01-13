#include <iostream>
#include <cstdlib>
#include <ctime>

#include "v6interface.h"
#include "jinglesession.h"

unsigned int _jingle_seed = 0;

using namespace gloox;


static const std::string jingle_session_reason_desc[] = { "undefined", 
		"alternative-session", "busy", "cancel", "connectivity-error", "decline", "expired", "failed-application", "failed-transport",
		"general-error", "gone", "incompatible-parameters", "media-error", "security-error", "success", "timeout", 
		"unsupported-applications", "unsupported-transports"
	};



JingleSession::JingleSession(ClientBase *base)
	: m_client(base), m_state(JSTATE_NULL), m_lastaction(ACTION_NONE)
{
	m_sid = m_client->getID();
	time((time_t *) &m_seed);
}

void JingleSession::parse(const Stanza *stanza, bool remote)
{
	Tag *jingle = stanza->findChild("jingle");
	if (!jingle)
		return;
	m_sid = jingle->findAttribute("sid");
	m_initiator = jingle->findAttribute("initiator");
	m_responder = jingle->findAttribute("responder");
	std::string action = jingle->findAttribute("action");
	m_lastaction = actionFromString(action);
        m_to = stanza->to();
        m_from = stanza->from();
        if (m_lastaction== ACTION_INITIATE) {
            m_caller = stanza->from();
            m_responder = stanza->to();
        } else if (m_lastaction == ACTION_ACCEPT) {
            m_caller = stanza->to();
            m_responder = stanza->from();
        }
	Tag::TagList contents = jingle->findChildren("content");
	
	for (Tag::TagList::iterator it=contents.begin(); it!=contents.end(); it++) {
		JingleContent content;
		content.parse(*it);
		if (remote) 
			addRemoteContent(content);
		else
			addLocalContent(content);
	}
}

void JingleSession::setJids(const JID &initiator, const JID &receiver)
{
	m_initiator = initiator;
	m_responder = receiver;
}

void JingleSession::addRemoteContent(const JingleContent &content)
{
	m_remote_contents.push_back(content);
}

void JingleSession::addParticipant(const gloox::JID &jid)
{
    m_participants.push_back(jid);
}

void JingleSession::addParticipant(const JingleParticipant &p)
{
    m_participants.push_back(p);
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
        m_pwd = tag->findAttribute("pwd");
        m_ufrag = tag->findAttribute("ufrag");
	Tag::TagList candidates = tag->findChildren("candidate");
	for (Tag::TagList::iterator it=candidates.begin(); it!=candidates.end(); ++it) {
		JingleUdpCandidate c;
		c.parse(*it);
		addCandidate(c);
	}
}

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


void JingleTransport::addCandidate(const JingleCandidate &c)
{
 	candidates.push_back(c);
}


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

void JingleRtpContentDescription::parse(const Tag *tag)
{
	if (!tag || tag->name() != "description")
		return;
	m_xmlns = tag->findAttribute("xmlns");
	m_media = tag->findAttribute("media");
	
	Tag::TagList payloads = tag->findChildren("payload");
	for (Tag::TagList::iterator it=payloads.begin(); it!=payloads.end(); ++it) {
		JingleRtpPayload payload(*it);
		addPayload(payload);
	}
}

Tag *JingleRtpContentDescription::tag() const
{
	Tag *t = new Tag("description");
	t->addAttribute("xmlns", m_xmlns);
	if (!m_media.empty())
		t->addAttribute("media", m_media);
	for (PayloadList::const_iterator it=payloads.begin(); it!=payloads.end(); ++it) {
		t->addChild(it->tag());
	}
	return t;
}

JingleRtpPayload::JingleRtpPayload(const Tag *tag)
    : id(0), clockrate(0), channels(1), maxptime(0), ptime(0)
{
    parse(tag);
}
	
JingleRtpPayload::JingleRtpPayload(unsigned char id, const std::string &name, unsigned int clockrate, int channels)
	: id(id), name(name), clockrate(clockrate), channels(channels), maxptime(0), ptime(0)
{
}

void JingleRtpPayload::parse(const Tag *tag)
{
	if(!tag || tag->name() != "payload")
		return;
	id = atoi( tag->findAttribute("id").c_str() );
	channels = atoi( tag->findAttribute("channels").c_str());
	name = tag->findAttribute("name");
	clockrate = atoi(tag->findAttribute("clockrate").c_str());
	ptime = atoi( tag->findAttribute("ptime").c_str());
	maxptime = atoi( tag->findAttribute("maxptime").c_str());
}

Tag * JingleRtpPayload::tag() const
{
	Tag *t = new Tag("payload");
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
	return t;
}

#if 0
JingleTransport::CandidateList JingleSession::localUdpCandidates()
{
	JingleTransport::CandidateList cl;
	char ** charlist = getAddressList(NULL, AFB_ANY, SCOPE_GLOBAL);
	if (!charlist) {
		std::cerr << "Chyba pri ziskavani lokalnich adres." << std::endl;
		return cl;
	}
	for (char **it=charlist; it && *it; it++) {
		JingleUdpCandidate candidate;
		candidate.ip = std::string(*it);
		candidate.port = randomPort();
		candidate.id = randomId();
		candidate.component = 0;
		candidate.generation = 0;
		candidate.natType = JingleCandidate::NAT_NONE;
		
	}
	v6Destroylist(charlist);
	return cl;
}

JingleTransport JingleSession::localTransport()
{
	JingleTransport t;
	t.m_xmlns = "urn:xmpp:jingle:transports:ice-udp:1";
	t.m_ufrag = randomId();
	t.m_pwd = randomId();
	t.candidates = localUdpCandidates();
	return t;
}

#endif

Tag * JingleContent::tag() const
{
	Tag *t = new Tag("content");
	t->addAttribute("xmlns", m_xmlns);
	if (!m_name.empty())
		t->addAttribute("name", m_name);
	if (!m_media.empty())
		t->addAttribute("media", m_media);
	if (!m_creator.empty())
		t->addAttribute("creator", m_creator);
	Tag *transport = m_transport.tag();
	if (transport)
		t->addChild(transport);
	Tag *description = m_description.tag();
	if (description)
		t->addChild(description);
	return t;
}

Tag * JingleSession::tag(SessionAction action) const
{
	Tag *t = new Tag("jingle");
	t->addAttribute("xmlns", XMLNS_JINGLE);
	t->addAttribute("sid", m_sid);
        if (action == ACTION_NONE)
            action = m_lastaction;
	switch (action) {
		case ACTION_INITIATE:	
                    t->addAttribute("action", "initiate"); break;
		case ACTION_ACCEPT: 
                    t->addAttribute("action", "accept"); break;
		case ACTION_TERMINATE:	
                    t->addAttribute("action", "terminate"); break;
		default: break;
	}
	
	// TODO: nejak vybrat action
	if (m_initiator)
		t->addAttribute("initiator", m_initiator.full());
	for (ContentList::const_iterator it=m_local_contents.begin(); 
            it!=m_local_contents.end(); 
            it++) {
		t->addChild((it)->tag());
	}
	return t;
}

JingleContent::JingleContent()
{
}

JingleContent::JingleContent(const JingleTransport &transport, const JingleRtpContentDescription &description)
	: m_transport(transport), m_description(description)
{
}


void JingleSession::addLocalContent(const JingleContent &content)
{
	m_local_contents.push_back(content);
}

/** @brief Prepare structure for initialization. */
int JingleSession::initiateAudioSession(const JID &from, const JID &to)
{
#ifdef REFACTOR
	JingleContent content( localTransport(), audioDescription() );
	m_initiator = from;
	m_caller = to;
	m_state = JSTATE_PENDING;
	m_sid = randomId();
        m_lastaction = ACTION_INITIATE;
	addContent(content);	
#endif
	return 1;
}

int JingleSession::acceptAudioSession(JingleSession *session)
{
        if (!session)
            return 0;
#ifdef REFACTOR
	JingleContent content( localTransport(), audioDescription() );
	m_state = JSTATE_ACTIVE;
	m_sid = session->sid();
        m_lastaction = ACTION_ACCEPT;
	addContent(content);
#endif
        return 1;
}

JingleSession::SessionAction JingleSession::actionFromString(const std::string &action)
{
	if (action == "initiate")
		return ACTION_INITIATE;
	else if (action == "accept")
		return ACTION_ACCEPT;
	else if (action == "terminate")
		return ACTION_TERMINATE;
	else
		return ACTION_NONE;
}

JingleSession::SessionReason JingleSession::reasonFromString(const std::string &reason)
{	
	size_t size = sizeof(jingle_session_reason_desc) / sizeof(std::string);
	for (size_t i =0; i< size; ++i) {
		if (reason == jingle_session_reason_desc[i]) {
			return ((SessionReason) i);
		}
	}
	return REASON_UNDEFINED;
}

std::string JingleSession::stringFromReason(SessionReason reason)
{
	size_t size = sizeof(jingle_session_reason_desc) / sizeof(std::string);
	size_t index = (size_t) reason;
	if (index <= size) 
		return jingle_session_reason_desc[index];
	else return std::string();
}

std::string JingleSession::stringFromState(SessionState state)
{
    switch (state) {
        case JSTATE_NULL:
            return "null";
        case JSTATE_ACTIVE:
            return "active";
        case JSTATE_PENDING:
            return "pending";
        case JSTATE_TERMINATED:
            return "terminated";
        default:
            return std::string();
    }
}

/** @brief Combine into this session remote content from given session.
    @param session parsed from incoming stanza
    @param remote if true, remote contents from session are put to my remote,
        in other case local contents from session are put to my remote contents.
*/
bool JingleSession::mergeSession(JingleSession *session, bool remote)
{
	if (!session)
		return false;
	if (m_sid != session->sid()) // session id does not match
		return false;
	if (session->m_responder)
		m_responder = session->m_responder;
	if (remote) {
		if (session->action() == ACTION_ACCEPT) {
			m_lastaction = session->action();
			m_remote_contents = session->m_remote_contents;
		}
	} else {
                m_remote_contents = session->m_local_contents;
        }
        return true;
}

gloox::JID  JingleSession::remote() const
{
    return m_remote_jid;
}

JingleSession * JingleSession::createReply(JingleSession *remote)
{
    if (!remote)
        return NULL;
    JingleSession *reply = new JingleSession(remote->m_client);
    reply->setSid(remote->sid());
    reply->setInitiator(remote->initiator());
    reply->setFrom(remote->to());
    reply->setTo(remote->from());
    reply->setRemote(remote->from());
    return reply;
}


void JingleSession::replaceRemoteContent(const ContentList &list)
{
    m_remote_contents = list;
}

void JingleSession::replaceLocalContent(const ContentList &list)
{
    m_local_contents = list;
}

void JingleSession::setCaller(bool caller)
{
    m_am_caller = caller;
}

std::string JingleSession::describe()
{
    std::string reply = "Jingle session id=\""+ m_sid + "\"\n";
    reply += "initiator: " + m_initiator.full() + "\n";
    reply += "from: " + m_from.full() + "\n";
    reply += "to: " + m_to.full() + "\n";
    reply += "remote: " + remote().full() + "\n";
    reply += "state: " + stringFromState(state()) + "\n";
    reply += "Local contents:\n";
    for (ContentList::const_iterator it=m_local_contents.begin(); 
            it!=m_local_contents.end(); it++) {
        gloox::Tag *t = it->tag();
        reply += "     "+ t->xml() + "\n";
        delete t;
    }
    reply += "Remote contents:\n";
    for (ContentList::const_iterator it=m_remote_contents.begin(); 
            it!=m_remote_contents.end(); it++) {
        gloox::Tag *t = it->tag();
        reply += "    "+ t->xml() + "\n";
        delete t;
    }
    return reply;
}
