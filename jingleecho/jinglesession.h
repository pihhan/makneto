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
#define XMLNS_JINGLE_RTP        "urn:xmpp:jingle:apps:rtp:1"
#define XMLNS_JINGLE_RAWUDP     "urn:xmpp:jingle:transports:raw-udp:1"
#define XMLNS_JINGLE_RTPINFO    "urn:xmpp:jingle:apps:rtp:info:1"


class JingleContentDescription
{
    public:
    std::string xmlns;
};

typedef enum {
        MEDIA_NONE = 0,
        MEDIA_AUDIO,
        MEDIA_VIDEO
} MediaType;

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

/** @brief Class for description of one content transfered over RTP protocol.
*/
class JingleRtpContentDescription : public JingleContentDescription
{
    public:

    typedef std::list<JingleRtpPayload> PayloadList;
	
	void addPayload(const JingleRtpPayload &payload) 
        { payloads.push_back(payload); }
	
	virtual void parse(const gloox::Tag *tag);
        virtual gloox::Tag *tag() const;

        std::string media() const
        { return m_media; }

        std::string xmlns() const
        { return m_xmlns; }

        MediaType   type() const
        { return m_type; }

    PayloadList payloads;
	std::string	m_xmlns;
	std::string m_media;
        MediaType   m_type;
};

/** @brief Class with parameters for one transport candidate for one content. 
*/
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

/** @brief One Transport representation with its candidates. 
    Transport is description of network connection.
    It contains network addresses and ports in candidates.
    There might me more than one candidate. 
*/
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

/** @brief One participant in session. */
class JingleParticipant
{
    public:
    typedef enum {
        TYPE_NONE = 0,
        TYPE_SEND,
        TYPE_RECEIVE,
        TYPE_BOTH
    } Types;

    JingleParticipant()
        : type(TYPE_NONE)
    {}

    JingleParticipant(const gloox::JID &jid) 
        : jid(jid), type(TYPE_BOTH)
    {
        nick = jid.bare();
    }

    gloox::JID  jid;
    std::string nick;
    Types       type;
};


typedef std::list<JingleParticipant>    ParticipantList;

    
/** @brief One stream of specified type, audio or video or something like it. */
class JingleContent
{
    public:
        typedef enum {
                SENDERS_UNKNOWN = 0,
                SENDERS_NONE,
                SENDERS_INITIATOR,
                SENDERS_RESPONDER,
                SENDERS_BOTH
        } Senders; /// what side will produce some data
        
        typedef enum {
                CREATOR_NONE = 0,
                CREATOR_INITIATOR,
                CREATOR_RESPONDER
        } Creator;


        typedef std::list<JingleTransport>  TransportList;
        
        JingleContent();
        JingleContent(const JingleTransport &transport, const JingleRtpContentDescription &description);

        JingleRtpContentDescription description() const;
        JingleTransport transport() const;
            
        void parse(const gloox::Tag *tag);
        gloox::Tag *tag() const;

        JingleParticipant owner()
            { return m_owner; }
        void setOwner( const JingleParticipant &p ) 
            { m_owner = p; }

        std::string name() const;
        Creator     creator() const;
        MediaType   media() const;
        std::string disposition() const;
        Senders     senders() const;

        void setName(const std::string &name);
        void setCreator(Creator creator);
        void setMedia(MediaType media);
        void setDisposition(const std::string &disposition);
        void setSenders(Senders s);

    private:
    JingleTransport     m_transport;
    JingleRtpContentDescription m_description;
    std::string m_xmlns;
    std::string m_name;
    Creator     m_creator; 
    MediaType   m_media;
    std::string m_disposition; // what type of content is inside
    Senders     m_senders;
    JingleParticipant   m_owner;
};

typedef std::list<JingleContent>	ContentList;

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
    ACTION_TRANSPORT_REPLACE,
    ACTION_LAST = ACTION_TRANSPORT_REPLACE
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

typedef enum {
    INFO_NONE = 0,
    INFO_ACTIVE,
    INFO_HOLD,
    INFO_MUTE,
    INFO_RINGING,
    INFO_LAST = INFO_RINGING
} SessionInfo;


/** @brief Class carrying all information about one stanza with Jingle namespace
 * */
class JingleStanza
{
    public:

    JingleStanza();

    std::string sid() const;
    ContentList contents() const;
    gloox::JID  from() const;
    gloox::JID  to() const;
    gloox::JID  responder() const;
    gloox::JID  initiator() const;

    SessionAction action() const;
    SessionReason reason() const;
    SessionInfo   info() const;
    std::string   reasonText() const;
    std::string   alternateSid() const;
    bool          valid() const;

    void setSid(const std::string &sid);
    void setContent( const ContentList &list);
    void setFrom(const gloox::JID &jid);
    void setTo(const gloox::JID &jid);
    void setResponder( const gloox::JID &jid);
    void setInitiator( const gloox::JID &jid);
    void setAction(SessionAction action);
    void setReason(SessionReason reason);
    void setInfo(SessionInfo info);
    void setReasonText(const std::string &text);
    void setAlternateSid(const std::string &sid);

    void parse(const gloox::Stanza *staza);
    /** @brief Get jingle tag for query. */
    gloox::Tag *tag() const;

    void addContent(const JingleContent &content);
    void addContent(const ContentList &contents);
    void replaceContents(const ContentList &contents);
    void clearContents();
    ContentList contents();
    JingleContent firstContent();

    private:
    ContentList     m_contents;

    gloox::JID      m_initiator;
    gloox::JID	m_caller;
    gloox::JID	m_responder;

    gloox::JID      m_from;
    gloox::JID      m_to;
        
    std::string m_sid;
    SessionAction   m_action;
    SessionReason   m_reason;
    SessionInfo     m_info;
    std::string     m_reasonText; 
    std::string     m_alternateSid; ///!< Valid only for alternative-session reason
    bool            m_valid;   
};


/** @brief One whole jingle session, ie. one audio/video call maybe. */
class JingleSession
{
    public:

    JingleSession(gloox::ClientBase *base);
	
    static JingleSession *createReply(JingleSession *remote);
	
    void parse(const gloox::Stanza *staza, bool remote=false);
    void addContent(const JingleContent &content) 
        { addLocalContent(content); };
    void addLocalContent(const JingleContent &content);

    void addRemoteContent(const JingleContent &content);
    void addParticipant(const gloox::JID &jid);
    void addParticipant(const JingleParticipant &p);
    void replaceRemoteContent(const ContentList &list);
    void replaceLocalContent(const ContentList &list);
	
    /** @brief Get jingle tag for query. */
    gloox::Tag *tag(SessionAction action = ACTION_NONE) const;
    
    std::string sid() const;
    SessionAction	action() const;
    SessionReason	reason() const;
    SessionState	state() const;
    SessionInfo     info() const;

    gloox::JID	    initiator() const;
    gloox::JID      caller() const;
    gloox::JID	    responder() const;
    gloox::JID      remote() const;
    gloox::JID      from() const;
    gloox::JID      to() const;
    bool        localOriginated() const;
	
    void setSid(const std::string &sid);
    void setJids(const gloox::JID &initiator, const gloox::JID &receiver);
    void setAction(SessionAction action);
    bool mergeSession(JingleSession *session, bool remote = true);
    void setSelf(const gloox::JID &self);
    void setInitiator(const gloox::JID &jid);
    void setCaller(const gloox::JID &jid);
    void setResponder(const gloox::JID &jid);
    void setRemote(const gloox::JID &jid);
    void setTo(const gloox::JID &jid);
    void setFrom(const gloox::JID &jid);
    void setCaller(bool caller);

    /** @brief Mark if last sent stanza has been acked. */
    void setAcknowledged(bool ack);
    
    void setState(SessionState state);
    
    /** @brief get context number for gloox matching to stanzas without sid. */
    int context();
    /** @brief Context for help with gloox matching with id. */
    void setContext(int context);

    ContentList     localContents();
    ContentList     remoteContents();

    JingleContent   firstRemoteContent();
    JingleContent   firstLocalContent();

    /** @brief Create string representation for human reading. */
    std::string     describe();

    void * data();
    void setData(void *data);

    JingleStanza *createStanza(SessionAction action);
    JingleStanza *createStanzaInitiate();
    JingleStanza *createStanzaInfo(SessionInfo info);
    JingleStanza *createStanzaAccept();
    JingleStanza *createStanzaTerminate(SessionReason reason = REASON_DECLINE);

    void initiateFromRemote(const JingleStanza *stanza);
    bool mergeFromRemote(const JingleStanza *session);

    public:
        static SessionAction actionFromString(const std::string &action);
        static SessionReason reasonFromString(const std::string &reason);
        static SessionInfo   infoFromString(const std::string &info);

        static std::string stringFromAction(SessionAction action);
        static std::string stringFromReason(SessionReason reason);
        static std::string stringFromState(SessionState state);
        static std::string stringFromInfo(SessionInfo info);

    private:	
    ContentList	m_local_contents;
    ContentList     m_remote_contents;
    gloox::JID      m_initiator;
    gloox::JID	m_caller;
    gloox::JID	m_responder;

    gloox::JID      m_my_jid;
    gloox::JID      m_remote_jid;
    gloox::JID      m_from;
    gloox::JID      m_to;
        
    std::string m_sid;
    gloox::ClientBase	*m_client;
    SessionState		m_state;
    SessionReason		m_reason;
    unsigned int		m_seed;
    SessionAction		m_lastaction;
    SessionInfo         m_info;
    ParticipantList         m_participants;
    bool				m_acknowledged; ///!< Whether we received acknowledge of last action from remote party. 
    int					m_context;
    bool                    m_am_caller; ///!< true if i am calling remote party, false if i accepted this session.
    /// not sure i need this global versioning
    int                     m_localversion; ///!< local version number of changes
    int                     m_ackedversion; ///!< version number we received acks from remote
    void                    *m_data; ///!< Custom data about this session

	
};


#endif

