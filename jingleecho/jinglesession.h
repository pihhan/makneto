/**

Repesent one jingle session with its state.

see http://xmpp.org/extensions/xep-0166.html
*/

#ifndef JINGLESESSION_H
#define JINGLESESSION_H

#include <string>
#include <list>

#ifdef GLOOX
#include <gloox/gloox.h>
#include <gloox/stanza.h>
#include <gloox/jid.h>
#include <gloox/clientbase.h>
#include <gloox/client.h>
#endif

#ifdef IRIS
#include <QDomElement>
#endif

#include "pjid.h"

#include "jingle-description.h"
#include "jingle-candidate.h"
#include "jingle-content.h"

#define XMLNS_JINGLE		"urn:xmpp:jingle:1"
#define XMLNS_JINGLE_RTP        "urn:xmpp:jingle:apps:rtp:1"
#define XMLNS_JINGLE_RTPINFO    "urn:xmpp:jingle:apps:rtp:info:1"


typedef enum { 
    JSTATE_NULL = 0,
    JSTATE_PRECONFIGURE, // wait for preparation of media pipeline
    JSTATE_PENDING, // after session-initiate
    JSTATE_ACTIVE, // after session-accept
    JSTATE_TERMINATED, // after session-terminate
    JSTATE_TIMEOUT, // did not receive reply from remote party in time.
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
    INFO_UNHOLD,
    INFO_UNMUTE,
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
    PJid  from() const;
    PJid  to() const;
    PJid  responder() const;
    PJid  initiator() const;

    SessionAction action() const;
    SessionReason reason() const;
    SessionInfo   info() const;
    std::string   reasonText() const;
    std::string   alternateSid() const;
    bool          valid() const;

    void setSid(const std::string &sid);
    void setContent( const ContentList &list);
    void setFrom(const PJid &jid);
    void setTo(const PJid &jid);
    void setResponder( const PJid &jid);
    void setInitiator( const PJid &jid);
    void setAction(SessionAction action);
    void setReason(SessionReason reason);
    void setInfo(SessionInfo info);
    void setReasonText(const std::string &text);
    void setAlternateSid(const std::string &sid);

#ifdef GLOOX
    void parse(const gloox::Stanza *staza);
    /** @brief Get jingle tag for query. */
    gloox::Tag *tag() const;
#endif
#ifdef IRIS
    void parse(const QDomElement &tag);
    QDomElement tag(QDomDocument &doc) const;
#endif

    void addContent(const JingleContent &content);
    void addContent(const ContentList &contents);
    void replaceContents(const ContentList &contents);
    void clearContents();
    ContentList contents();
    JingleContent firstContent();

    private:
    ContentList     m_contents;

    PJid      m_initiator;
    PJid	m_caller;
    PJid	m_responder;

    PJid      m_from;
    PJid      m_to;
        
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

    JingleSession();
	
    static JingleSession *createReply(JingleSession *remote);
	
    void addContent(const JingleContent &content) 
        { addLocalContent(content); };
    void addLocalContent(const JingleContent &content);

    void addRemoteContent(const JingleContent &content);
    void addParticipant(const PJid &jid);
    void addParticipant(const JingleParticipant &p);
    void replaceRemoteContent(const ContentList &list);
    void replaceLocalContent(const ContentList &list);
	
    /** @brief Get jingle tag for query. */
#ifdef GLOOX
    void parse(const gloox::Stanza *staza, bool remote=false);
    gloox::Tag *tag(SessionAction action = ACTION_NONE) const;
#endif
#ifdef IRIS
#endif
    
    std::string sid() const;
    SessionAction	action() const;
    SessionReason	reason() const;
    SessionState	state() const;
    SessionInfo     info() const;

    PJid	    initiator() const;
    PJid      caller() const;
    PJid	    responder() const;
    PJid      remote() const;
    PJid      from() const;
    PJid      to() const;
    bool        localOriginated() const;
	
    void setSid(const std::string &sid);
    void setJids(const PJid &initiator, const PJid &receiver);
    void setAction(SessionAction action);
    bool mergeSession(JingleSession *session, bool remote = true);
    void setSelf(const PJid &self);
    void setInitiator(const PJid &jid);
    void setCaller(const PJid &jid);
    void setResponder(const PJid &jid);
    void setRemote(const PJid &jid);
    void setTo(const PJid &jid);
    void setFrom(const PJid &jid);
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

    bool failed() const;
    void setFailed(bool failed);

    JingleStanza *createStanza(SessionAction action);
    JingleStanza *createStanzaInitiate();
    JingleStanza *createStanzaInfo(SessionInfo info);
    JingleStanza *createStanzaAccept();
    JingleStanza *createStanzaTerminate(SessionReason reason = REASON_DECLINE);

    void initiateFromRemote(const JingleStanza *stanza);
    bool mergeFromRemote(const JingleStanza *session);

    /** @brief Generate random id. */
    std::string randomId();

    int localContentsWithType(MediaType type);
    int remoteContentsWithType(MediaType type);

    bool codecsAreCompatible();
    bool codecsAreCompatible(
            const JingleContent &local, 
            const JingleContent &remote);
    bool needLocalPayload();


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
    PJid      m_initiator;
    PJid	m_caller;
    PJid	m_responder;

    PJid      m_my_jid;
    PJid      m_remote_jid;
    PJid      m_from;
    PJid      m_to;
        
    std::string m_sid;
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
    bool                    m_failed;
	
};


#endif

