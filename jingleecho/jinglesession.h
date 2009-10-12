/**

Repesent one jingle session with its state.

see http://xmpp.org/extensions/xep-0166.html
*/

#ifndef JINGLESESSION_H
#define JINGLESESSION_H

#include <string>
#include <list>

#include <gloox/gloox.h>
#include <gloox/stanza.h>
#include <gloox/jid.h>
#include <gloox/clientbase.h>
#include <gloox/client.h>

#define XMLNS_JINGLE		"urn:xmpp:jingle:1"
#define XMLNS_JINGLE_ICE	"urn:xmpp:jingle:transports:ice-udp:1"


class JingleContentDescription
{
    public:
    std::string xmlns;
};


/** Class describing one RTP payload for session negotiation.
    @see http://xmpp.org/extensions/xep-0167.html
	It describes one format the client is able to handle for RTP session
    */
class JingleRtpPayload
{
    public:
		JingleRtpPayload(unsigned char id, const std::string &name, clockrate=8000, channels=1);
		
    unsigned char   id; ///<! required
    std::string     name; ///!< recommended for static, required for dynamic
    unsigned int    clockrate; ///!< recommended
    int             channels; ///!< recommended
    unsigned int    maxptime; ///!< optional
    unsigned int    ptime; ///!< optional
	
	void parse(const gloox::Tag *tag);
	
	gloox::Tag *tag();

};

class JingleRtpContentDescription : public JingleContentDescription
{
    public:

    typedef std::list<JingleRtpPayload> PayloadList;
	
	void addPayload(JingleRtpPayload &payload) { payloads.push_back(payload); }
	
	virtual void parse(const gloox::Tag *tag);

    PayloadList payloads;
	std::string	m_xmlns;
	std::string m_media;
};

class JingleCandidate
{
    public:
    typedef enum { 
        NAT_NONE,
        NAT_SYMMETRIC,
        NAT_PERMISSIVE
    } NatType;
	
	virtual void parse(const gloox::Tag *tag);
	virtual gloox::Tag *tag();

    int             component;
    std::string     ip;
    std::string     id;
    unsigned int    port;
    int             generation;
    NatType         natType;
};

/** @brief Representation of one candidate for raw-UDP transport.
@see http://xmpp.org/extensions/xep-0177.html
*/
class JingleUdpCandidate : public JingleCandidate
{
    public:

};

/** @brief Representation of one candidate for ICE transport.
@see http://xmpp.org/extensions/xep-0176.html
 Only ICE specific things are added.
*/
class JingleIceCandidate : public JingleCandidate
{
	typedef enum {
		PR_UNSPECIFIED = 0,
		PR_UDP,
		PR_TCP
	} Protocols;
	
	virtual void parse(const gloox::Tag *tag);
	
    int     foundation;
    int     network;
    int     priority;
    std::string     protocol;
    std::string     type;

    std::string     relAddr;
    std::string     relPort;
};

/** @brief One Transport representation with its candidates. */
class JingleTransport
{
    public:
		typedef std::list<JingleCandidate *>	CandidateList;
		
		
		virtual void parse(const gloox::Tag *tag);
		void addCandidate(JingleCandidate *c);
		
    std::string xmlns;
    std::string pwd;
    std::string ufrag;
	CandidateList	candidates;
};

/** @brief One stream of specified type, audio or video or something like it. */
class JingleContent
{
    public:
		
		JingleContentTransport m_transport;
		JingleRtpContentDescription m_description;
		
		JingleContent();
		JingleContent(JingleContentTransport &transport, JingleRtpContentDescription &description);
		
		void parse(const gloox::Tag *tag);
		gloox::Tag *tag();

    //private:
    std::string m_xmlns;
    std::string m_name;
    std::string m_media;
    std::string m_creator;
};

/** @brief One whole jingle session, ie. one audio/video call maybe. */
class JingleSession
{
    public:

    typedef enum { 
        JSTATE_NULL = 0,
        JSTATE_PENDING, // after session-initiate
        JSTATE_ACTIVE, // after session-accept
        JSTATE_TERMINATED, // after session-terminate
    } SessionState;
	typedef enum {
		ACTION_NONE = 0,
		ACTION_INITIATE,
		ACTION_ACCEPT,
		ACTION_TERMINATE
	} SessionAction;
	typedef std::list<JingleContent *>	ContentList;
	
	

    JingleSession(gloox::ClientBase *base);
	
	void parse(const gloox::Stanza *staza);
	
	unsigned int randomPort();
	std::string	randomId();
	
	/** @brief Get list of IPs this machine has. */
	JingleTransport::CandidateList					localUdpCandidates();
	JingleTransport					localTransport();
	
	JingleRtpContentDescription		audioDescription();
	
	int initiateAudioSession(const gloox::JID &from, const gloox::JID &to);
	void addContent(const JingleContent &content);
	
	/** @brief Get jingle tag for query. */
	gloox::Tag *tag(SessionAction action = ACTION_INITIATE);
		
		
	ContentList	m_contents;
    gloox::JID  m_initiator;
	gloox::JID	m_target;
    std::string m_sid;
	gloox::ClientBase	*m_client;
	SessionState		m_state;
	unsigned int		m_seed;
};

#endif

