
#include <cstdlib>
#include <list>
#include <vector>
#include <iostream>

#include <gloox/disco.h>

#include "requests.h"
#include "client.h"
#include "parser.h"
#include "logger.h"

using namespace gloox;

RequestList::RequestList(EchoClient *client)
    : m_client(client), m_requests( vectorSize )
{
}

bool RequestList::contextPresent(int context)
{
    if ((size_t) context > vectorSize)
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
    if ((size_t) context > vectorSize)
        return Request();
    return m_requests.at(context);
}


void RequestList::handleDiscoInfoResult(Stanza *stanza, int context)
{
    Request r = getRequest(context);
    if (r.type != Request::DISCO_INFO) { 
        std::cerr << "Zadost o disco nema spravny typ kontextu!" << std::endl;
        return;
    }

        std::string result("Disco#info for ");
        result.append(stanza->from() + "\n");

        Tag * query = stanza->findChild("query");
        if (!query) {
            m_client->sendChatMessage(r.from, "(empty info)");
            return;
        }
        Tag::TagList identities = query->findChildren("identity");
        Tag::TagList features = query->findChildren("feature");
        result.append("Identities: \n");
        for (Tag::TagList::const_iterator it=identities.begin(); it!=identities.end(); it++) {
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
    if (r.type == Request::DISCO_ITEMS) {
        std::string result("Disco#items for ");
        result.append(stanza->from() + "\n");

        Tag * query = stanza->findChild("query");
        if (!query) {
            m_client->sendChatMessage(r.from, "(no items)");
            return;
        }
        Tag::TagList items = query->findChildren("item");
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
    CL_DEBUG(m_client->client(), "Disco error occured from " + stanza->from());
    if (r.type == Request::DISCO_ITEMS || r.type == Request::DISCO_INFO) {
        std::string desc("Disco error from ");
        desc.append(stanza->from() + ": " );
		desc.append(EchoClient::describeError(stanza));
        m_client->sendChatMessage(r.from, desc);
    }
}

bool RequestList::handleDiscoSet(Stanza *stanza)
{
    return false;
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

void RequestList::createVersionRequest(gloox::JID origin, gloox::JID target)
{
    Request r = createRequest(Request::VERSION);
    r.from = origin;
    r.to = target;
    addRequest(r);
	m_client->versionHandler()->requestVersion(target, this, r.context);
}

void RequestList::handleVersion(gloox::Stanza *stanza, int context)
{
	Request r = getRequest(context);
	if (r.type == Request::VERSION) {
		if (stanza->subtype() == StanzaIqResult) {
			std::string name = getVerName(stanza);
			std::string version = getVerNumber(stanza);
			std::string os = getVerOs(stanza);
			std::string reply = "Name: " + name + " Version: " + version + " OS: "+ os;
			m_client->sendChatMessage(r.from, reply);
		} else if (stanza->subtype() == StanzaIqError) {
			std::string reply = EchoClient::describeError(stanza);
			m_client->sendChatMessage(r.from, reply);
		}
	}
}

void RequestList::addRequest(Request r)
{
    m_requests[r.context] = r;
}

void RequestList::removeRequest(Request r)
{
	m_requests[r.context] = Request();
}

void RequestList::handleAdhocCommand(const std::string &command, Tag *tag, const gloox::JID &from, const std::string &id)
{
    std::cout << "command " << command << " from " << from.full() << std::endl;
}


/*
 * Single Request entry.
 */


Request::Request()
    : context(0), type(NONE), data(0)
{
}



/* jingle stuff */

JingleSession::SessionReason RequestList::handleNewSession(JingleSession *session)
{
	std::string from = session->initiator().full();
        std::string sid;
        if (session) 
            sid = session->sid();
	m_client->broadcastChatMessage("Incoming call from "+ from 
                + " with id:" + sid);
	
	Request r = createRequest(Request::JINGLE);
	r.to = session->initiator();
	r.data = (void *) session;
	session->setContext(r.context);
        addRequest(r);
        return JingleSession::REASON_SUCCESS;
}

JingleSession::SessionReason RequestList::handleSessionAccept(JingleSession *session, JingleSession *update)
{
    
    m_client->broadcastChatMessage("Session accepted");
    std::cout << "handler JingleSessionAccept" << __FUNCTION__ << std::endl;
    return JingleSession::REASON_UNDEFINED;
}

JingleSession::SessionReason RequestList::handleSessionChange(JingleSession *session, JingleSession *update)
{
    std::cout << "handler " << __FUNCTION__ << std::endl;
    return JingleSession::REASON_UNDEFINED;
}

JingleSession::SessionReason RequestList::handleSessionTermination(JingleSession *session)
{
    std::string sid;
    if (session)
        sid = session->sid();
    std::cout << "handler " << __FUNCTION__ << std::endl;
    m_client->broadcastChatMessage("Session id:"+ sid + " terminated.");
    return JingleSession::REASON_UNDEFINED;
}

JingleSession::SessionReason RequestList::handleSessionError(JingleSession *session, const gloox::Stanza *stanza)
{
    std::string sid;
    if (session) {
        m_client->broadcastChatMessage("Jingle Error for session id:" 
                + session->sid());
    } else {
        m_client->broadcastChatMessage("Jingle Error came from " + stanza->from().full()
                + " without session");
    }
    std::cout << "handler " << __FUNCTION__ << std::endl;
    return JingleSession::REASON_UNDEFINED;
}
    
Request RequestList::createJingleRequest(const JID &origin, JingleSession *session)
{
    Request r = createRequest(Request::JINGLE);
    r.data = session;
    return r;
}

