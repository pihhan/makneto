
#ifndef JINGLE_CONTENT_H
#define JINGLE_CONTENT_H

#include "pjid.h"

#include "jingle-description.h"
#include "jingle-transport.h"

#ifdef IRIS
#include <QDomElement>
#endif

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

    JingleParticipant(const PJid &jid) 
        : jid(jid), type(TYPE_BOTH)
    {
        nick = jid.bareStd();
    }

    PJid  jid;
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
       
#ifdef GLOOX
        void parse(const gloox::Tag *tag);
        gloox::Tag *tag() const;
#else
        void parse(const QDomElement &tag);
        QDomElement tag(QDomDocument &doc) const;
#endif

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
        void setTransport(JingleTransport &transport);
        void setDescription(JingleRtpContentDescription &desc);

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


#endif

