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
    int             channels; ///!< recommended
    unsigned int    clockrate; ///!< recommended
    unsigned char   id; ///<! required
    unsigned int    maxptime; ///!< optional
    std::string     name; ///!< recommended for static, required for dynamic
    unsigned int    ptime; ///!< optional
	
	void parse(const gloox::Tag *tag);

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
	
	virtual void parse(const gloox::Tag *tag)

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
		
		/** @brief Get list of IPs this machine has. */
		CandidateList localUdpCandidates();
		
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
		
		void parse(const gloox::Tag *tag);

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
		ACTION_INITIATE,
		ACTION_ACCEPT,
		ACTION_TERMINATE
	} SessionAction;
	typedef std::list<JingleContent *>	ContentList;
	
	

    JingleSession();
	
	void parse(const gloox::Stanza *staza);

	ContentList	m_contents;
    gloox::JID  m_initiator;
    std::string m_sid;
};

#endif

