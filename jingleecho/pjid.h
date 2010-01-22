/*
 My own abstraction to Jabber ID, to unify JID access methods between
 JID in gloox and Jid in iris.
 */

#ifndef PJID_H
#define PJID_H

#include <string>

#define GLOOX
#ifdef GLOOX
#include <gloox/jid.h>
#endif

#ifdef GLOOX
class PJid  : public gloox::JID
#elif defined(IRIS)
class PJid  : public XMPP::Jid
#else
#error "No GLOOX or IRIS defined"
#endif
{
    public:

    PJid(const std::string &fulljid);

    std::string fullStd() const;
    std::string bareStd() const; 
    std::string resourceStd() const;

#ifdef IRIS
    PJid(const QString &fulljid);
    PJid(const XMPP::Jid &jid);
    const std::string & username() const;
    const std::string & server() const;
#endif 

#ifdef GLOOX
    PJid(const gloox::JID &jid);
#endif
};

#endif
