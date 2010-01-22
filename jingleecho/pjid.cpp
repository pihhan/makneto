
#ifdef IRIS
#include <xmpp/jid.h>
#endif

#ifdef GLOOX
#include <gloox/jid.h>
#endif


#ifdef IRIS
PJid::PJid(const std::string &fulljid)
    : XMPP::Jid(fulljid.c_str())
{
}

PJid::PJid(const QString &fulljid)
    : XMPP::Jid(fulljid)
{

}
    
PJid::PJid(const XMPP::Jid &jid)
    : XMPP::Jid(jid)
{
}

std::string PJid::username() const
{
    return node().toStdString();
}

std::string PJid::server() const
{
    return domain().toStdString();
}

std::string PJid::resourceStd() const
{
    return resource().toStdString();
}

std::string PJid::fullStd() const
{
    return full().toStdString();
}

std::string PJid::bareStd() const
{
    return bare().toStdString();
}
#endif

#ifdef GLOOX
PJid::PJid(const std::string &fulljid)
    : gloox::JID(fulljid)
{
}

PJid::PJid(const gloox::JID &jid)
    : gloox::JID(jid)
{
}

std::string PJid::resourceStd() const
{
    return resource();
}

std::string PJid::fullStd() const
{
    return gloox::JID.full();
}

std::string PJid::bareStd() const
{
    return gloox::JID.bare();
}

#endif
