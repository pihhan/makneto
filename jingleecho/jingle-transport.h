
#ifndef JINGLE_TRANSPORT_H
#define JINGLE_TRANSPORT_H

#include "jingle-candidate.h"

/** @brief One Transport representation with its candidates. 
    Transport is description of network connection.
    It contains network addresses and ports in candidates.
    There might me more than one candidate. 
*/
class JingleTransport
{
    public:
    
#ifdef GLOOX
    virtual void parse(const gloox::Tag *tag);
    gloox::Tag * tag() const;
#else
    virtual void parse(const QDomElement &tag);
    QDomElement tag(QDomDocument &doc) const;
#endif

    void addCandidate(const JingleCandidate &c);

    std::string xmlns();
    void setXmlns(const std::string &ns);
		
    std::string m_xmlns;
    std::string m_pwd;
    std::string m_ufrag;
    CandidateList	candidates;
};



#endif
