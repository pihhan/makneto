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

class JingleContentDescription
{
    public:
    std::string xmlns;
};


/** Class describing one RTP payload for session negotiation.
    @see http://xmpp.org/extensions/xep-0167.html
    */
class JingleRTPPayload
{
    public:
    int             channels; ///!< recommended
    unsigned int    clockrate; ///!< recommended
    unsigned char   id; ///<! required
    unsigned int    maxptime; ///!< optional
    std::string     name; ///!< recommended for static, required for dynamic
    unsigned int    ptime; ///!< optional

};

class JingleRTPContentDescription : public JingleContentDescription
{
    public:

    typedef std::list<JingleRTPPayload> PayloadList;

    PayloadList payloads;
};



/** @brief Representation of one candidate for raw-UDP transport.
@see http://xmpp.org/extensions/xep-0177.html
*/
class JingleUdpCandidate
{
    public:
    typedef enum { 
        NAT_NONE,
        NAT_SYMMETRIC,
        NAT_PERMISSIVE
    } NatType;

    int             component;
    std::string     ip;
    std::string     id;
    unsigned int    port;
    int             generation;
    NatType         natType;
};

/** @brief Representation of one candidate for ICE transport.
@see http://xmpp.org/extensions/xep-0176.html
*/

class JingleIceCandidate
{
    int     component;
    int     generation;
    int     foundation;
    std::string id;
    std::string ip;
    int     network;
    unsigned int    port;
    int     priority;
    std::string     protocol;
    std::string     type;

    std::string     relAddr;
    std::string     relPort;
};


class JingleContentTransport
{
    public:
    std::string xmlns;
    std::string pwd;
    std::string ufrag;

};

class JingleSession
{
    public:

    typedef enum { 
        JSTATE_NULL = 0,
        JSTATE_PENDING, // after session-initiate
        JSTATE_ACTIVE, // after session-accept
        JSTATE_TERMINATED, // after session-terminate
    } SessionState;

    JingleSession();

    private:
    gloox::JID  m_initiator;
    std::string m_sid;
};

class JingleContent
{
    public:

    private:
    std::string m_xmlns;
    std::string m_name;
    std::string m_media;
    std::string m_creator;
};

#endif

