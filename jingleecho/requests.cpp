
#include <cstdlib>
#include <list>
#include <vector>

#include "requests.h"
#include "client.h"
#include "parser.h"
#include "logger.h"


RequestList::RequestList(EchoClient *client)
    : m_client(client), m_requests( vectorSize )
{
}

bool RequestList::contextPresent(int context)
{
    if (context > vectorSize)
        return false;
    return (m_requests.at(context).type != Request::NONE);
}

/** @brief Generate unique context number. */
int RequestList::generateContext()
{
    int r;
    do {
        r = rand() % vectorSize;
    } while (contextPresent(r));
    return r;
}

Request RequestList::getRequest(int context)
{
    if (context > vectorSize)
        return Request();
    return m_requests.at(context);
}


void RequestList::handleDiscoInfoResult(Stanza *stanza, int context)
{
    Request r = getRequest(context);
    if (r.type != DISCO_INFO) { 
        std::cerr << "Zadost o disco nema spravny typ kontextu!" << std::endl;
        return;
    }

        std::string result("Disco#info for ");
        result.append(stanza->from() + "\n");

        Tag * query = stanza->findChild(query);
        Tag::TagList identities = query->findChildren("identity");
        Tag::TagList features = query->findChildren("features");
        result.append("Identities: \n");
        for (Tag::TagList::const_iterator it=identites.begin(); it!=identities.end(); it++) {
            Tag *t = (*it);
            std::string one = t->findAttribute("category") + "/" +
                t->findAttribute("type") + "/" + t->findAttribute("name") + "\n";
            result.append(one);
        }
        result.append("Features: \n");
        for (Tag::TagList::const_iterator it=features.begin(); it!=features.end(); it++) {
            Tag *t = (*it);
            result.append(t->findAttribute("var") + ", ");
        }
        m_client->sendChatMessage(r.from, result);
}

void RequestList::handleDiscoItemsResult(Stanza *stanza, int context)
{
    Request r = getRequest(context);
    if (r.type == Request::ITEMS) {
        std::string result("Disco#items for ");
        result.append(stanza->from() + "\n");

        Tag * query = stanza->findChild(query);
        Tag::TagList items = query->findChildren("items");
        result.append("Items: \n");
        for (Tag::TagList::const_iterator it=items.begin(); it!=items.end(); it++) {
            Tag *t = (*it);
            std::string one = t->findAttribute("jid") + ": " +
                t->findAttribute("name") + "\n";
            result.append(one);
        }
        m_client->sendChatMessage(r.from, result);
    } else {
    }
}

void RequestList::handleDiscoError(Stanza *stanza, int context)
{
    Request r = getRequest(context);
    CL_LOG(m_client->client(), "Disco error occured from " + stanza->from());
    if (r.type == DISCO_ITEMS || r.type == DISCO_INFO) {
        m_client->sendChatMessage(r.from, "Disco error from " 
            + stanza->from() + ": " + stanza->errorText());
    }
}


Request RequestList::createRequest(Request::RequestType t)
{
    Request r;
    r.type = t;
    r.context = generateContext();
    return r;
}

void RequestList::createDiscoRequest(gloox::JID origin, gloox::JID target, const std::string &node)
{
    Request r1 = createRequest(Request::DISCO_INFO);
    Request r2 = createRequest(Request::DISCO_ITEMS);
    r1.from = origin;
    r2.from = origin;
    r1.to = target;
    r2.to = target;

    addRequest(r1);
    addRequest(r2);
    m_client->client()->disco()->getDiscoInfo(target, node, this, r1.context);
    m_client->client()->disco()->getDiscoItems(target,node, this, r2.context);
}

void RequestList::addRequest(Request r)
{
    m_requests[r.context] = r;
}


Request::Request()
    : context(0), type(NONE)
{
}

