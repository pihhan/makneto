
#ifndef JINGLE_CANDIDATE_H
#define JINGLE_CANDIDATE_H

#ifdef GLOOX
#include <gloox/tag.h>
#else
#include <QDomElement>
#endif
#include <list>

#define XMLNS_JINGLE_ICE	"urn:xmpp:jingle:transports:ice-udp:1"
#define XMLNS_JINGLE_RAWUDP     "urn:xmpp:jingle:transports:raw-udp:1"

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

    typedef enum {
        TYPE_UNKNOWN = 0,
        TYPE_HOST, ///!< IP address of machine itself
        TYPE_REFLEXIVE, ///!< public IP address of NAT box, my source address in internet
        TYPE_RELAYED, ///!< Address of third party relay
        TYPE_PEER_REFLEXIVE, ///!< Reflexive address got from peer
    } CandidateType;

    typedef enum {
        REACHABLE_UNKNOWN = 0, ///!< We don't know as we haven't try yet
        REACHABLE_NO, ///!< We tried and failed
        REACHABLE_YES, ///!< We tried and got response
        REACHABLE_TRYING ///!< We don't know yet, but we are trying now
    } ReachableType;
	

#ifdef GLOOX
    virtual void parse(const gloox::Tag *tag);
    virtual gloox::Tag *tag() const;
#else
    virtual void parse(const QDomElement &tag);
    virtual QDomElement tag(QDomDocument &doc) const;
#endif

    JingleCandidate();
    virtual std::string xmlns() const;
    void increaseGeneration();

    int             component;
    std::string     ip;
    std::string     id;
    unsigned int    port;
    int             generation;
    NatType         natType;
    CandidateType   candidateType;
    ReachableType   reachable;
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
    public:

	typedef enum {
		PR_UNSPECIFIED = 0,
		PR_UDP,
		PR_TCP
	} Protocols;

#ifdef GLOOX_API	
    virtual void parse(const gloox::Tag *tag);
    virtual gloox::Tag *tag() const;
#else
    virtual void parse(const QDomElement &tag);
    virtual QDomElement tag(QDomDocument &doc) const;
#endif

    JingleIceCandidate();
    JingleIceCandidate(const JingleCandidate &candidate);

    JingleCandidate operator=(const JingleCandidate &c);

    virtual std::string xmlns() const;
    	
    int     foundation;
    int     network;
    Protocols     protocol;
    int     priority;
    std::string     type;

    std::string     relAddr;
    unsigned int    relPort;
};

typedef std::list<JingleCandidate>	CandidateList;

/** @brief Class to hold local and remote pairs. */
class JingleCandidatePair
{
    public:
    JingleCandidatePair();

    JingleCandidate local;
    JingleCandidate remote;
};


#endif

