/*
 My own abstraction to Jabber ID, to unify JID access methods between
 JID in gloox and Jid in iris.
 */

#ifndef PJID_H
#define PJID_H

#include <string>

#ifdef GLOOX
#include <gloox/jid.h>
#endif
#ifdef IRIS
#include <iris/xmpp_jid.h>
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

    PJid();
    PJid(const std::string &fulljid);

    std::string fullStd() const;
    std::string bareStd() const; 
    std::string resourceStd() const;

#ifdef IRIS
    PJid(const QString &fulljid);
    PJid(const XMPP::Jid &jid);
    PJid(const char *fulljid);
    std::string username() const;
    std::string server() const;

    operator bool() const;
#endif 

#ifdef GLOOX
    PJid(const gloox::JID &jid);
#endif
};

#endif
