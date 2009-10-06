#include <string>

#include "versionhandler.h"

#define XMLNS_VERSION2 "jabber:iq:version"

using namespace gloox;

VersionReceiver::VersionReceiver()
{
}

std::string VersionReceiver::getVerName(Stanza *stanza)
{
    Tag *query = stanza->findChild("query");
    if (query) {
        Tag *name = query->findChild("name");
        if (name)
            return name->cdata();
    }
    return std::string();
}

std::string VersionReceiver::getVerNumber(Stanza *stanza)
{
    Tag *query = stanza->findChild("query");
    if (query) {
        Tag *ver = query->findChild("version");
        if (ver)
            return ver->cdata();
    }
    return std::string();
}

std::string VersionReceiver::getVerOs(Stanza *stanza)
{
    Tag *query = stanza->findChild("query");
    if (query) {
        Tag *os = query->findChild("os");
        if (os)
            return os->cdata();
    }
    return std::string();
}

    
VersionIqHandler::VersionIqHandler(ClientBase *base)
    : m_base(base)
{

}

bool VersionIqHandler::handleIq(Stanza *stanza)
{
    VersionReceiver *rcvr;
    switch (stanza->subtype()) {
        case StanzaIqGet:
            replyVersion(stanza);
            return true;
            break;
        case StanzaIqSet:
            break;
        case StanzaIqError:
        case StanzaIqResult:
            rcvr = m_contexts(0);
            if (rcvr) {
                rcvr->handleVersion(stanza, 0);
                return true;
            }
            break;
    }
    return false;
}

bool VersionIqHandler::handleIqID(Stanza *stanza, int context)
{
    VersionReceiver *rcvr;
    switch (stanza->subtype()) {
        case StanzaIqGet:
            replyVersion(stanza);
            return true;
            break;
        case StanzaIqSet:
            break;
        case StanzaIqError:
        case StanzaIqResult:
            rcvr = m_contexts(context);
            if (rcvr) {
                rcvr->handleVersion(stanza, context);
                return true;
            }
            break;
    }
    return false;
}

void VersionIqHandler::requestVersion(const gloox::JID &target, VersionReceiver *rcvr, int context)
{
    if ((size_t) context > m_contexts.capacity()) {
        m_cotexts.resize(context, NULL);
    }
    m_contexts.at(context) = rcvr;

    std::string id(m_base->getID());
    Stanza *req = Stanza::createIqStanza(target, id, StanzaIqGet, XMLNS_VERSION1);
    m_base->send(req); 

    m_base->trackID(this, id, context);
}

void VersionIqHandler::replyVersion(const gloox::Stanza *stanza)
{
    std::string id = m_base->getID();
    Stanza *reply = Stanza::createIqStanza(stanza->from(), id, StanzaIqResult, XMLNS_VERSION1);
    Tag *query = reply->findChild("query");
    new Tag(query, "name", m_name);
    new Tag(query, "version", m_version);
    if (!m_os.empty())
        new Tag(query, "os", m_os);
    stanza->finalize();
    m_base->send(reply);
}

void VersionIqHandler::setVersion(const std::string &version, const std::string &name, const std::string &os)
{
    m_version = version;
    m_name = name;
    m_os = os;
}

