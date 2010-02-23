#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include "jinglesession.h"
#include "logger/logger.h"

#define RANDOM_ID_LENGTH 8
unsigned int _jingle_seed = 0;

#ifdef GLOOX
using namespace gloox;
#endif

static const std::string jingle_session_reason_desc[] = { "undefined", 
		"alternative-session", "busy", "cancel", "connectivity-error", "decline", "expired", "failed-application", "failed-transport",
		"general-error", "gone", "incompatible-parameters", "media-error", "security-error", "success", "timeout", 
		"unsupported-applications", "unsupported-transports"
	};

static const std::string jingle_session_info_desc[] = {
    "none", "active", "hold", "mute", "ringing", "unhold", "unmute", "!LAST!"
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

JingleSession::JingleSession()
	: m_state(JSTATE_NULL), m_lastaction(ACTION_NONE), m_am_caller(false),
          m_data(0), m_failed(false)
{
	time((time_t *) &m_seed);
        m_sid = randomId();
}

std::string JingleSession::randomId()
{
    static const char base[] = "01234567890abcdefghijklmnopqrstuvwxyz";
    std::string id;
    for (int len = 0; len<RANDOM_ID_LENGTH; len++) {
        int i = rand_r(&m_seed) % (sizeof(base)-1);
        id.append(1, base[i]);
    }
    return id;
}

#ifdef GLOOX
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
#endif

void JingleSession::setJids(const PJid &initiator, const PJid &receiver)
{
	m_initiator = initiator;
	m_responder = receiver;
}

void JingleSession::addRemoteContent(const JingleContent &content)
{
	m_remote_contents.push_back(content);
}

void JingleSession::addParticipant(const PJid &jid)
{
    m_participants.push_back(jid);
}

void JingleSession::addParticipant(const JingleParticipant &p)
{
    m_participants.push_back(p);
}



/*
 *
 * JingleSession
 *
 */

#ifdef GLOOX
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
#endif // GLOOX

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

JingleSession * JingleSession::createReply(JingleSession *remote)
{
    if (!remote)
        return NULL;
    JingleSession *reply = new JingleSession();
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
    reply += "initiator: " + m_initiator.fullStd() + "\n";
    reply += "from: " + m_from.fullStd() + "\n";
    reply += "to: " + m_to.fullStd() + "\n";
    reply += "remote: " + remote().fullStd() + "\n";
    reply += "state: " + stringFromState(state()) + "\n";
    reply += "Local contents:\n";
    for (ContentList::const_iterator it=m_local_contents.begin(); 
            it!=m_local_contents.end(); it++) {
#ifdef GLOOX
        gloox::Tag *t = it->tag();
        reply += "     "+ t->xml() + "\n";
        delete t;
#else
        QDomDocument doc;
        QDomElement e = it->tag(doc);
        reply += "     " + e.text().toStdString() + "\n";
#endif
    }
    reply += "Remote contents:\n";
    for (ContentList::const_iterator it=m_remote_contents.begin(); 
            it!=m_remote_contents.end(); it++) {
#ifdef GLOOX
        gloox::Tag *t = it->tag();
        reply += "     "+ t->xml() + "\n";
        delete t;
#else
        QDomDocument doc;
        QDomElement e = it->tag(doc);
        reply += "     " + e.text().toStdString() + "\n";
#endif
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

bool JingleSession::failed() const
{
    return m_failed;
}

void JingleSession::setFailed(bool failed)
{
    m_failed = failed;
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
    
JingleStanza * JingleSession::createStanzaTerminate(SessionReason reason)
{
    JingleStanza *stanza = createStanza(ACTION_TERMINATE);
    stanza->setReason(reason);
    return stanza;
}

/** @brief Initiate session from incoming session-initiate request. */
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

/** @brief Update remote candidates from incoming session-accept stanza. */
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


PJid JingleSession::initiator() const	
{
    return m_initiator; 
}

PJid JingleSession::caller() const          
{
    return m_caller; 
}

PJid JingleSession::responder() const	
{
    return m_responder; 
}

PJid JingleSession::remote() const
{
    return m_remote_jid;
}

PJid JingleSession::from() const    
{
    return m_from; 
}

PJid JingleSession::to() const      
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

void JingleSession::setSelf(const PJid &self)
{
     m_my_jid = self; 
}

void JingleSession::setInitiator(const PJid &jid)
{
     m_initiator = jid; 
}

void JingleSession::setCaller(const PJid &jid)
{
     m_caller = jid; 
}

void JingleSession::setResponder(const PJid &jid)
{
     m_responder = jid; 
}

void JingleSession::setRemote(const PJid &jid)
{
     m_remote_jid = jid; 
}

void JingleSession::setTo(const PJid &jid)
{
     m_to = jid; 
}

void JingleSession::setFrom(const PJid &jid)
{
     m_from = jid; 
}

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

int JingleSession::localContentsWithType(MediaType type)
{
    int count = 0;
    for (ContentList::iterator it=m_local_contents.begin(); 
        it!=m_local_contents.end(); it++) 
    {
        if (it->description().type() == type)
            ++count;
    }
    return count;
}

int JingleSession::remoteContentsWithType(MediaType type)
{
    int count = 0;
    for (ContentList::iterator it=m_remote_contents.begin(); 
        it!=m_remote_contents.end(); it++) 
    {
        if (it->description().type() == type)
            ++count;
    }
    return count;
}

bool JingleSession::codecsAreCompatible(
    const JingleContent &local, 
    const JingleContent &remote)
{
    JingleRtpContentDescription ld = local.description();
    JingleRtpContentDescription rd = remote.description();
    int matches = 0;
    PayloadList lp = ld.payloads;
    PayloadList rp = rd.payloads;
    for (PayloadList::iterator l = lp.begin(); l!=lp.end(); l++) {
        for (PayloadList::iterator r = rp.begin(); r!=rp.end(); r++) {
            if (l->name == r->name)
                matches++;
        }

    }
    return (matches>0);
}

/** @brief Check whether remote and local codecs are compatible. */
bool JingleSession::codecsAreCompatible()
{
    bool compatible = true;
    for (ContentList::iterator li=m_local_contents.begin(); 
        li!=m_local_contents.end(); li++) {

        for (ContentList::iterator ri=m_remote_contents.begin(); 
            ri!=m_remote_contents.end(); ri++) {
            if (li->description().type() == ri->description().type()) {
                compatible = compatible && codecsAreCompatible(*li, *ri);
            }
        }
    }
    return compatible;
}

/** @brief Check whether contents need payload. 
    @return true if at least one content does not have any payload 
    in description, false otherwise. */
bool JingleSession::needLocalPayload()
{
    bool haspayload = true;
    for (ContentList::iterator it=m_local_contents.begin(); 
        it!=m_local_contents.end(); it++) 
    {
        JingleRtpContentDescription d = it->description();
        if (d.countPayload() <= 0) {
            haspayload = false;
        }
    }
    return (!haspayload);
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


#ifdef GLOOX
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
    m_reason = REASON_UNDEFINED;
    if (jingle->hasChild("reason")) {
        Tag *reason = jingle->findChild("reason");
        for (Tag::TagList::iterator it = reason->children().begin();
                it != reason->children().end(); it++) {
            if (!(*it)->hasAttribute("xmlns")) {
                SessionReason r = JingleSession::reasonFromString((*it)->name());
                if (r != REASON_UNDEFINED)
                    m_reason = r;
                if (r == REASON_ALTERNATIVE_SESSION) {
                    Tag *sid = (*it)->findChild("sid");
                    if (sid)
                        m_alternateSid = sid->cdata();
                }

            } // !xmlns
        } // for

        Tag *text = reason->findChild("text");
        if (text)
            m_reasonText = text->cdata();
    }
    m_valid = true;
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

    if (m_reason != REASON_UNDEFINED) {
        Tag *reason = new Tag("reason");
        std::string rstr = JingleSession::stringFromReason(m_reason);
        Tag *child = new Tag(reason, rstr);
        if (m_reason == REASON_ALTERNATIVE_SESSION && !m_alternateSid.empty()) {
            new Tag(child, "sid", m_alternateSid);
        }
        if (!m_reasonText.empty())
            new Tag(reason, "text", m_reasonText);
    }
    return t;
}

#else

/** @brief Parse incoming XML stanza to structure. 
    Qt/Iris version.
*/
void JingleStanza::parse(const QDomElement &tag)
{
    QDomElement jingle = tag.firstChildElement("jingle");
    if (jingle.isNull())
        return;
    m_sid = jingle.attribute("sid").toStdString();
    m_initiator = jingle.attribute("initiator").toStdString();
    m_responder = jingle.attribute("responder").toStdString();
    std::string action = jingle.attribute("action").toStdString();
    m_action = JingleSession::actionFromString(action);

    m_from = tag.attribute("from");
    m_to = tag.attribute("to");

    QDomElement content = jingle.firstChildElement("content");
    while (!content.isNull()) {
        JingleContent c;
        c.parse(content);
        addContent(c);
        content = content.nextSiblingElement("content");
    }

    m_reason = REASON_UNDEFINED;
    QDomElement reason = jingle.firstChildElement("reason");
    if (!reason.isNull()) {
        QDomElement rchild = reason.firstChildElement();
        while (!rchild.isNull()) {
            if (!rchild.hasAttribute("xmlns")) {
                std::string tagname = rchild.tagName().toStdString();
                SessionReason r = JingleSession::reasonFromString(tagname);

                if (r != REASON_UNDEFINED)
                    m_reason = r;
                if (r == REASON_ALTERNATIVE_SESSION) {
                    QDomElement sid = rchild.firstChildElement("sid");
                    if (!sid.isNull())
                        m_alternateSid = sid.text().toStdString();
                }
            }
        } // while rchild
    } // if reason 

}

/**
    @TODO Tag je vracen jako <jingle>, ale parsuje se <iq>
*/
QDomElement JingleStanza::tag(QDomDocument &doc) const
{
    QDomElement t = doc.createElement("jingle");
    t.setAttribute("xmlns", XMLNS_JINGLE);
    t.setAttribute("sid", m_sid.c_str());

    std::string straction = JingleSession::stringFromAction(m_action);
    if (!straction.empty()) 
            t.setAttribute("action", straction.c_str());
    else LOGGER(logit) << "Prazdne action po prevedeni na retezec" 
            << m_action << std::endl;
            
    if (m_initiator)
        t.setAttribute("initiator", m_initiator.full());
    if (m_responder)
        t.setAttribute("responder", m_responder.full());

    for (ContentList::const_iterator it=m_contents.begin(); 
        it!=m_contents.end(); 
        it++) {
            t.appendChild((it)->tag(doc));
    }

    if (m_info != INFO_NONE) {
        std::string infostr = JingleSession::stringFromInfo(m_info);
        QDomElement info = doc.createElement("info");
        info.setAttribute("xmlns", XMLNS_JINGLE_RTPINFO);
        t.appendChild(info);
    }

    if (m_reason != REASON_UNDEFINED) {
        QDomElement reason = doc.createElement("reason");
        std::string rstr = JingleSession::stringFromReason(m_reason);
        QDomElement child = doc.createElement(rstr.c_str());
        if (m_reason == REASON_ALTERNATIVE_SESSION && !m_alternateSid.empty()) {
            QDomElement sid = doc.createElement("sid");
            QDomText sidtext = doc.createTextNode(m_alternateSid.c_str());
            sid.appendChild(sidtext);
            child.appendChild(sid);
        }
        if (!m_reasonText.empty()) {
            QDomElement rtext = doc.createElement("text");
            QDomText rtextt = doc.createTextNode(m_reasonText.c_str());
            rtext.appendChild(rtextt);
            child.appendChild(rtext);
        }
        reason.appendChild(child);
    }
    return t;
}
#endif

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

std::string JingleStanza::sid() const 
{ return m_sid; }

ContentList JingleStanza::contents() const 
{ return m_contents; }

PJid  JingleStanza::from() const 
{ return m_from; }

PJid  JingleStanza::to() const          
{ return m_to; }

PJid  JingleStanza::responder() const   
{ return m_responder; }

PJid  JingleStanza::initiator() const   
{ return m_initiator; }

SessionAction JingleStanza::action() const    
{ return m_action; }

SessionReason JingleStanza::reason() const
{ return m_reason; }

SessionInfo   JingleStanza::info() const
{ return m_info; }

std::string   JingleStanza::reasonText() const
{
    return m_reasonText;
}

/** @brief Alternative session id.
    Valid only for alternative-session reason. */
std::string   JingleStanza::alternateSid() const
{
    return m_alternateSid;
}

bool JingleStanza::valid() const 
{
    return m_valid;
}


void JingleStanza::setSid(const std::string &sid)
{ m_sid = sid; }

void JingleStanza::setContent( const ContentList &list)
{ m_contents = list; }

void JingleStanza::setFrom(const PJid &jid)
{ m_from = jid; }

void JingleStanza::setTo(const PJid &jid)
{ m_to = jid; }

void JingleStanza::setResponder( const PJid &jid)
{ m_responder = jid; }

void JingleStanza::setInitiator( const PJid &jid)
{ m_initiator = jid; }

void JingleStanza::setAction(SessionAction action)
{ m_action = action; }

void JingleStanza::setReason(SessionReason reason)
{ m_reason = reason; }

void JingleStanza::setInfo(SessionInfo info)
{ m_info = info; }

void JingleStanza::setReasonText(const std::string &text)
{ m_reasonText = text; }

void JingleStanza::setAlternateSid(const std::string &sid)
{
    m_alternateSid = sid;
}

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


