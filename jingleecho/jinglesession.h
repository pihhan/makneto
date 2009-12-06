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
                JingleRtpPayload(const gloox::Tag *tag);
		JingleRtpPayload(unsigned char id, const std::string &name, unsigned int clockrate=8000, int channels=1);
		
    unsigned char   id; ///<! required
    std::string     name; ///!< recommended for static, required for dynamic
    unsigned int    clockrate; ///!< recommended
    int             channels; ///!< recommended
    unsigned int    maxptime; ///!< optional
    unsigned int    ptime; ///!< optional
	
	void parse(const gloox::Tag *tag);
	
	gloox::Tag *tag() const;

};

class JingleRtpContentDescription : public JingleContentDescription
{
    public:

    typedef std::list<JingleRtpPayload> PayloadList;
	
	void addPayload(const JingleRtpPayload &payload) { payloads.push_back(payload); }
	
	virtual void parse(const gloox::Tag *tag);
        virtual gloox::Tag *tag() const;

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
	virtual gloox::Tag *tag() const;

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

        virtual gloox::Tag *tag() const;
	
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
		typedef std::list<JingleCandidate>	CandidateList;
		
		
		virtual void parse(const gloox::Tag *tag);
		void addCandidate(const JingleCandidate &c);

                gloox::Tag * tag() const;
		
    std::string m_xmlns;
    std::string m_pwd;
    std::string m_ufrag;
	CandidateList	candidates;
};

/** @brief One stream of specified type, audio or video or something like it. */
class JingleContent
{
    public:
		typedef enum {
			SENDER_NONE = 0,
			SENDER_INITIATOR,
			SENDER_RESPONDER,
			SENDER_BOTH
		} Senders; /// what side will produce some data
		
		typedef enum {
			CREATOR_INITIATOR,
			CREATOR_RESPONDER
		} Creator;

                typedef std::list<JingleTransport>  TransportList;
		
		JingleContent();
		JingleContent(const JingleTransport &transport, const JingleRtpContentDescription &description);

		JingleTransport m_transport;
		JingleRtpContentDescription m_description;


                JingleRtpContentDescription description() { return m_description; }
		
		void parse(const gloox::Tag *tag);
		gloox::Tag *tag() const;

    //private:
    std::string m_xmlns;
    std::string m_name;
    std::string m_media;
    std::string m_creator; // 
	std::string m_disposition; // what type of content is inside
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
		ACTION_TERMINATE,
		ACTION_INFO,
		ACTION_CONTENT_ADD,
		ACTION_CONTENT_MODIFY,
		ACTION_CONTENT_ACCEPT,
		ACTION_CONTENT_REMOVE,
		ACTION_CONTENT_REJECT,
		ACTION_DESCRIPTION_INFO,
		ACTION_SECURITY_INFO,
		ACTION_TRANSPORT_ACCEPT,
		ACTION_TRANSPORT_REJECT,
		ACTION_TRANSPORT_INFO,
		ACTION_TRANSPORT_REPLACE
	} SessionAction;
	
	typedef enum {
		REASON_UNDEFINED = 0,
		REASON_ALTERNATIVE_SESSION,
		REASON_BUSY,
		REASON_CANCEL,
		REASON_CONECTIVITY_ERROR,
		REASON_DECLINE,
		REASON_EXPIRED,
		REASON_FAILED_APPLICATION,
		REASON_FAILED_TRANSPORT,
		REASON_GENERAL_ERROR,
		REASON_GONE,
		REASON_INCOMPATIBLE_PARAMETERS,
		REASON_MEDIA_ERROR,
		REASON_SECURITY_ERROR,
		REASON_SUCCESS,
		REASON_UNSUPPORTED_APPLICATIONS,
		REASON_UNSUPPORTED_TRANSPORTS
	} SessionReason;
	typedef std::list<JingleContent>	ContentList;
	
	

    JingleSession(gloox::ClientBase *base);
	
	void parse(const gloox::Stanza *staza, bool remote=false);
	
	unsigned int randomPort();
	std::string	randomId();
	
	/** @brief Get list of IPs this machine has. */
	JingleTransport::CandidateList					localUdpCandidates();
	JingleTransport					localTransport();
	
	JingleRtpContentDescription		audioDescription();
        JingleContent                           audioContent();
	
	int initiateAudioSession(const gloox::JID &from, const gloox::JID &to);
        int acceptAudioSession(JingleSession *session);
	void addContent(const JingleContent &content);
	void addRemoteContent(const JingleContent &content);
	
	/** @brief Get jingle tag for query. */
	gloox::Tag *tag(SessionAction action = ACTION_INITIATE) const;
	
	std::string sid() { return m_sid; }
	SessionAction	action() const 	{ return m_lastaction; }
	SessionReason	reason() const	{ return m_reason; }
	SessionState	state() const	{ return m_state; }
	gloox::JID		initiator() const	{ return m_initiator; }
	gloox::JID		responder() const	{ return m_responder; }
	
	void setJids(const gloox::JID &initiator, const gloox::JID &receiver);
        void setAction(SessionAction action) { m_lastaction = action; }
	bool mergeSession(JingleSession *session, bool remote = true);
	
	void setAcknowledged(bool ack)
	{ m_acknowledged = ack; }
	
	void setState(SessionState state)
	{ m_state = state; }
	
	int context()
	{ return m_context; }
	void setContext(int context)
	{ m_context = context; }

        ContentList     contents() { return m_contents; }
		
	ContentList	m_contents;
	ContentList m_remote_contents;
    gloox::JID  m_initiator;
	gloox::JID	m_target;
	gloox::JID	m_responder;
    std::string m_sid;
	gloox::ClientBase	*m_client;
	SessionState		m_state;
	SessionReason		m_reason;
	unsigned int		m_seed;
	SessionAction		m_lastaction;
	bool				m_acknowledged; ///!< Whether we received acknowledge of last action from remote party. 
	int					m_context;
	
	protected:
		static SessionAction actionFromString(const std::string &action);
		static SessionReason reasonFromString(const std::string &reason);
        public:
		static std::string stringFromReason(SessionReason reason);
};

#endif

