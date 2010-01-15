#include <iostream>
#include <cstdlib>
#include <ctime>

#include "jinglesession.h"
#include "logger/logger.h"

unsigned int _jingle_seed = 0;

using namespace gloox;


static const std::string jingle_session_reason_desc[] = { "undefined", 
		"alternative-session", "busy", "cancel", "connectivity-error", "decline", "expired", "failed-application", "failed-transport",
		"general-error", "gone", "incompatible-parameters", "media-error", "security-error", "success", "timeout", 
		"unsupported-applications", "unsupported-transports"
	};

static const std::string jingle_session_info_desc[] = {
    "none", "active", "hold", "mute", "ringing", "!LAST!"
};

static const std::string action_descriptions[] = { 
            "none","session-initiate", 
            "session-accept", "session-terminate", "session-info",
            "content-add", "content-modify", "content-accept", 
            "content-remove", "content-reject", "description-info",
            "security-info", "transport-accept", "transport-reject",
            "transport-info", "transport-replace", "!ACTION-LAST!"
};

/*
 *
 * JingleSession
 *
 */

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

/*
 *
 * JingleContent
 *
 */

/** @brief Create content without any contents */
JingleContent::JingleContent()
    : m_creator(CREATOR_NONE), m_media(MEDIA_NONE)
{
}

/** @brief Create content from given transport and description parts. 
    @param transport Transport class.
    @param description Description class.
    */
JingleContent::JingleContent(const JingleTransport &transport, const JingleRtpContentDescription &description)
	: m_transport(transport), m_description(description),
          m_creator(CREATOR_NONE), m_media(MEDIA_NONE)
{
}

/** @brief Parse XML subtree of tag <content> to this structure. */
void JingleContent::parse(const Tag *tag)
{
	if (!tag || tag->name() != "content")
		return;
	m_name = tag->findAttribute("name");
        m_xmlns = tag->findAttribute("xmlns");

	std::string media = tag->findAttribute("media");
        if (media == "audio")
            m_media = MEDIA_AUDIO;
        else if (media == "video")
            m_media = MEDIA_VIDEO;
        else
            m_media = MEDIA_NONE;

	std::string creator = tag->findAttribute("creator");
        if (creator == "initiator")
            m_creator = CREATOR_INITIATOR;
        else if (creator == "responder")
            m_creator = CREATOR_RESPONDER;
        else
            m_creator = CREATOR_NONE;

	std::string m_disposition = tag->findAttribute("disposition");
	
	Tag *desc = tag->findChild("description");

	m_description = JingleRtpContentDescription();
	m_description.parse(desc);
	Tag *transport = tag->findChild("transport");
	m_transport = JingleTransport();
	m_transport.parse(transport);
}

JingleRtpContentDescription JingleContent::description() const
{ 
    return m_description;
}

JingleTransport JingleContent::transport() const
{ 
    return m_transport;
}

/** @brief Create xml subtree for content tag. */
Tag * JingleContent::tag() const
{
    Tag *t = new Tag("content");
    t->addAttribute("xmlns", m_xmlns);
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

    Tag *transport = m_transport.tag();
    if (transport)
            t->addChild(transport);
    Tag *description = m_description.tag();
    if (description)
            t->addChild(description);
    return t;
}

std::string JingleContent::name() const
{
    return m_name;
}

Creator     JingleContent::creator() const
{
    return m_creator;
}

MediaType   JingleContent::media() const
{
    return m_media;
}

std::string JingleContent::disposition() const
{
    return m_disposition;
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
    m_media = media;
}

void JingleContent::setDisposition(const std::string &disposition)
{
    m_disposition = disposition;
}


/*
 *
 * Jingle Transport
 *
 */

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

void JingleTransport::addCandidate(const JingleCandidate &c)
{
 	candidates.push_back(c);
}

/*
 *
 * JingleCandidate
 *
 */

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

/*
 *
 * JingleIceCandidate
 *
 */

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

/*
 *
 * JingleRtpContentDescription
 *
 */

/** @brief Fill class from given XML subtree, tag must be named <description> */
void JingleRtpContentDescription::parse(const Tag *tag)
{
	if (!tag || tag->name() != "description")
		return;
	m_xmlns = tag->findAttribute("xmlns");
	m_media = tag->findAttribute("media");
        if (m_media == "audio") {
            m_type = TYPE_AUDIO;
        } else if (m_media == "video") {
            m_type = TYPE_VIDEO;
        }
	
	Tag::TagList payloads = tag->findChildren("payload");
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
	if (!m_media.empty())
		t->addAttribute("media", m_media);
	for (PayloadList::const_iterator it=payloads.begin(); it!=payloads.end(); ++it) {
		t->addChild(it->tag());
	}
	return t;
}

/*
 *
 * JingleRtpPayload
 *
 */

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


/*
 *
 * JingleSession
 *
 */

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

void JingleSession::addLocalContent(const JingleContent &content)
{
	m_local_contents.push_back(content);
}

/** @brief Create enum value from text value used in jingle action
    parameter. 
    @param action value of action attribute, inside jingle tag. */
SessionAction JingleSession::actionFromString(const std::string &action)
{
	if (action == "session-initiate")
		return ACTION_INITIATE;
	else if (action == "session-accept")
		return ACTION_ACCEPT;
	else if (action == "session-terminate")
		return ACTION_TERMINATE;
    else if (action == "session-info")
        return ACTION_INFO;
    else if (action == "content-accept")
        return ACTION_CONTENT_ACCEPT;
    else if (action == "content-add")
        return ACTION_CONTENT_ADD;
    else if (action == "content-modify")
        return ACTION_CONTENT_MODIFY;
    else if (action == "content-reject")
        return ACTION_CONTENT_REJECT;
    else if (action == "content-remove")
        return ACTION_CONTENT_REMOVE;
    else if (action == "description-info")
        return ACTION_DESCRIPTION_INFO;
    else if (action == "transport-info")
        return ACTION_TRANSPORT_INFO;
    /* TODO: transport-{accept,reject,replace} */
	else
		return ACTION_NONE;
}

SessionReason JingleSession::reasonFromString(const std::string &reason)
{	
	size_t size = sizeof(jingle_session_reason_desc) / sizeof(std::string);
	for (size_t i =0; i< size; ++i) {
		if (reason == jingle_session_reason_desc[i]) {
			return ((SessionReason) i);
		}
	}
	return REASON_UNDEFINED;
}

SessionInfo JingleSession::infoFromString(const std::string &info)
{
    size_t size = sizeof(jingle_session_info_desc) / sizeof(std::string);
    for (size_t i=0; i < size; ++i) {
        if (info == jingle_session_info_desc[i]) 
            return (SessionInfo) i;
    }
    return INFO_NONE;
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

std::string JingleSession::stringFromInfo(SessionInfo info)
{
	size_t size = sizeof(jingle_session_info_desc) / sizeof(std::string);
	size_t index = (size_t) info;
	if (index <= size) 
		return jingle_session_reason_desc[index];
	else return std::string();
}

std::string JingleSession::stringFromAction(SessionAction action)
{
    int index;
    if (action >= ACTION_NONE && action <=ACTION_LAST) {
        index = (int) action;
        return action_descriptions[index];
    }
    return std::string();
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

void * JingleSession::data()
{
    return m_data;
}

void JingleSession::setData(void *data)
{
    m_data = data;
}

JingleContent JingleSession::firstRemoteContent()
{
    if (m_remote_contents.size() > 0) {
        return m_remote_contents.front();
    } else {
        return JingleContent();
    }
}

JingleContent JingleSession::firstLocalContent()
{
    if (m_local_contents.size() > 0) {
        return m_local_contents.front();
    } else {
        return JingleContent();
    }
}

JingleStanza * JingleSession::createStanza(SessionAction action)
{
    JingleStanza *stanza = new JingleStanza();
    stanza->setFrom(m_my_jid);
    stanza->setTo(m_remote_jid);
    stanza->setAction(action);
    stanza->setSid(m_sid);
    return stanza;
}

JingleStanza * JingleSession::createStanzaInitiate()
{
    JingleStanza *stanza = createStanza(ACTION_INITIATE);
    stanza->setInitiator(m_initiator);
    if (m_local_contents.size() > 0)
        stanza->addContent(m_local_contents);

    return stanza;
}

JingleStanza * JingleSession::createStanzaAccept()
{
    JingleStanza *stanza = createStanza(ACTION_ACCEPT);
    if (m_responder)
        stanza->setResponder(m_responder);
    if (m_local_contents.size() > 0)
        stanza->addContent(m_local_contents);

    return stanza;
}

JingleStanza * JingleSession::createStanzaInfo(SessionInfo info)
{
    JingleStanza *stanza = createStanza(ACTION_INFO);
    stanza->setInfo(info);
    return stanza;
}

void JingleSession::initiateFromRemote(const JingleStanza *stanza)
{
    m_sid = stanza->sid();
    m_remote_jid = stanza->from();
    m_my_jid = stanza->to();
    m_initiator = stanza->initiator();
    m_responder = stanza->responder();
    m_remote_contents = stanza->contents();
    m_lastaction = stanza->action(); 
    m_from = stanza->from();
    m_to = stanza->to();
}

bool JingleSession::mergeFromRemote(const JingleStanza *stanza)
{
	if (!stanza)
		return false;
	if (m_sid != stanza->sid()) // session id does not match
		return false;
	if (stanza->responder())
		m_responder = stanza->responder();

    switch (stanza->action()) {
        case ACTION_ACCEPT:
            m_lastaction = stanza->action();
            replaceRemoteContent(stanza->contents());
            break;
        default:
            std::cerr << "Neobslouzena jingle akce v mergeFromRemote" << std::endl;
    }
    return true;
}

std::string JingleSession::sid() const
{
    return m_sid; 
}

SessionAction JingleSession::action() const 	
{
    return m_lastaction; 
}

SessionReason JingleSession::reason() const	
{
    return m_reason; 
}

SessionState JingleSession::state() const	
{
    return m_state; 
}

SessionInfo JingleSession::info() const    
{
    return m_info; 
}


gloox::JID JingleSession::initiator() const	
{
    return m_initiator; 
}

gloox::JID JingleSession::caller() const          
{
    return m_caller; 
}

gloox::JID JingleSession::responder() const	
{
    return m_responder; 
}

gloox::JID JingleSession::remote() const
{
    return m_remote;
}

gloox::JID JingleSession::from() const    
{
    return m_from; 
}

gloox::JID JingleSession::to() const      
{
    return m_to; 
}

bool JingleSession::localOriginated() const 
{
    return m_am_caller; 
}

void JingleSession::setSid(const std::string &sid)
{
     m_sid = sid; 
}

void JingleSession::setAction(SessionAction action)
{
     m_lastaction = action; 
}

void JingleSession::setSelf(const gloox::JID &self)
{
     m_my_jid = self; 
}

void JingleSession::setInitiator(const gloox::JID &jid)
{
     m_initiator = jid; 
}

void JingleSession::setCaller(const gloox::JID &jid)
{
     m_caller = jid; 
}

void JingleSession::setResponder(const gloox::JID &jid)
{
     m_responder = jid; 
}

void JingleSession::setRemote(const gloox::JID &jid)
{
     m_remote_jid = jid; 
}

void JingleSession::setTo(const gloox::JID &jid)
{
     m_to = jid; 
}

void JingleSession::setFrom(const gloox::JID &jid)
{
     m_from = jid; 
}

void JingleSession::setAcknowledged(bool ack)
{ m_acknowledged = ack; }

void JingleSession::setState(SessionState state)
{ m_state = state; }

int JingleSession::context()
{ return m_context; }

void JingleSession::setContext(int context)
{ m_context = context; }

ContentList JingleSession::localContents()
{
     return m_local_contents; 
}

ContentList JingleSession::remoteContents()
{
     return m_remote_contents; 
}

void JingleSession::setAcknowledged(bool ack)
{
    m_acknowledged = ack;
}



/*
 *
 * JingleStanza
 *
 */

JingleStanza::JingleStanza() 
    : m_action(ACTION_NONE), m_reason(REASON_UNDEFINED), m_info(INFO_NONE),
      m_valid(false)
{
}

/** @brief Parse XML stanza to structure. */
void JingleStanza::parse(const Stanza *stanza)
{
	Tag *jingle = stanza->findChild("jingle");
	if (!jingle) {
        m_valid = false;
		return;
    }
	m_sid = jingle->findAttribute("sid");
	m_initiator = jingle->findAttribute("initiator");
	m_responder = jingle->findAttribute("responder");
	std::string action = jingle->findAttribute("action");
	m_action = JingleSession::actionFromString(action);
    m_to = stanza->to();
    m_from = stanza->from();

	Tag::TagList contents = jingle->findChildren("content");
	
	for (Tag::TagList::iterator it=contents.begin(); it!=contents.end(); it++) {
		JingleContent content;
		content.parse(*it);
		addContent(content);
	}
    m_valid = true;
}

void JingleStanza::addContent(const JingleContent &content)
{
    m_contents.push_back(content);
}

void JingleStanza::addContent(const ContentList &contents)
{
    for (ContentList::const_iterator it= contents.begin();
                it != contents.end(); it++) {
        m_contents.push_back(*it);
    }
}


void JingleStanza::clearContents()
{
    m_contents.clear();
}

Tag * JingleStanza::tag() const
{
	Tag *t = new Tag("jingle");
	t->addAttribute("xmlns", XMLNS_JINGLE);
	t->addAttribute("sid", m_sid);

    std::string straction = JingleSession::stringFromAction(m_action);
    if (!straction.empty()) 
            t->addAttribute("action", straction);
    else LOGGER(logit) << "Prazdne action po prevedeni na retezec" 
            << m_action << std::endl;
            
	if (m_initiator)
		t->addAttribute("initiator", m_initiator.full());
    if (m_responder)
        t->addAttribute("responder", m_responder.full());
	for (ContentList::const_iterator it=m_contents.begin(); 
            it!=m_contents.end(); 
            it++) {
		t->addChild((it)->tag());
	}
    if (m_info != INFO_NONE) {
        std::string infostr = JingleSession::stringFromInfo(m_info);
        new Tag(t, infostr, "xmlns", XMLNS_JINGLE_RTPINFO);
    }
	return t;
}

std::string JingleStanza::sid() const 
{ return m_sid; }

ContentList JingleStanza::contents() const 
{ return m_contents; }

gloox::JID  JingleStanza::from() const 
{ return m_from; }

gloox::JID  JingleStanza::to() const          
{ return m_to; }

gloox::JID  JingleStanza::responder() const   
{ return m_responder; }

gloox::JID  JingleStanza::initiator() const   
{ return m_initiator; }

SessionAction JingleStanza::action() const    
{ return m_action; }

SessionReason JingleStanza::reason() const
{ return m_reason; }

SessionInfo   JingleStanza::info() const
{ return m_info; }

bool JingleStanza::valid() const 
{
    return m_valid;
}


void JingleStanza::setSid(const std::string &sid)
{ m_sid = sid; }

void JingleStanza::setContent( const ContentList &list)
{ m_contents = list; }

void JingleStanza::setFrom(const gloox::JID &jid)
{ m_from = jid; }

void JingleStanza::setTo(const gloox::JID &jid)
{ m_to = jid; }

void JingleStanza::setResponder( const gloox::JID &jid)
{ m_responder = jid; }

void JingleStanza::setInitiator( const gloox::JID &jid)
{ m_initiator = jid; }

void JingleStanza::setAction(SessionAction action)
{ m_action = action; }

void JingleStanza::setReason(SessionReason reason)
{ m_reason = reason; }

void JingleStanza::setInfo(SessionInfo info)
{ m_info = info; }

ContentList JingleStanza::contents()
{
    return m_contents;
}

JingleContent   JingleStanza::firstContent()
{
    if (m_contents.size() > 0)
        return m_contents.front();
    else 
        return JingleContent();
}

void JingleStanza::replaceContents(const ContentList &contents)
{
    m_contents = contents;
}

