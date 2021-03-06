
#ifndef REQUESTS_H
#define REQUESTS_H

#include <list>
#include <vector>

#include <gloox/gloox.h>
#include <gloox/jid.h>
#include <gloox/discohandler.h>
#include <gloox/adhochandler.h>
#include <gloox/adhoccommandprovider.h>

#include "versionhandler.h"
#include "jinglesession.h"
#include "jinglemanager.h"

namespace gloox {
    class EchoClient;
}

/** @brief Store one request and who initiated it. */
class Request
{
    public:
    typedef enum {
        NONE,
        DISCO_ITEMS,
        DISCO_INFO,
        VCARD,
        VERSION,
	JINGLE
    } RequestType;

    Request();

    gloox::JID  from;
    gloox::JID  to;
    int         context;
    RequestType type;
	void *		data;
};

class RequestList : public gloox::DiscoHandler, 
        public gloox::AdhocCommandProvider,
		public VersionReceiver,
		public JingleActionHandler
{
    public:
    typedef std::vector<Request> RequestVector;
    static const size_t    vectorSize = 1000;

    RequestList(gloox::EchoClient *client);

    virtual void handleDiscoInfoResult(gloox::Stanza *stanza, int context);
    virtual void handleDiscoItemsResult(gloox::Stanza *stanza, int context);
    virtual void handleDiscoError(gloox::Stanza *stanza, int context);
    virtual bool handleDiscoSet(gloox::Stanza *stanza);

    virtual void handleAdhocCommand(const std::string &command, gloox::Tag *tag, const gloox::JID &from, const std::string &id);

    int generateContext();
    Request getRequest(int context);
    bool    contextPresent(int context);
    Request createRequest(Request::RequestType t);
    void createDiscoRequest(gloox::JID origin, gloox::JID target, const std::string &node = std::string());
    void createVersionRequest(gloox::JID origin, gloox::JID target);
    /** @brief Only allocate request, not any network action. */
    Request createJingleRequest(const gloox::JID &origin, JingleSession *session);

	virtual void handleVersion(gloox::Stanza *stanza, int context);
	
	/* jingle stuff */
		SessionReason handleNewSession(JingleSession *session);
		SessionReason handleSessionAccept(JingleSession *session, JingleSession *update);
		SessionReason handleSessionChange(JingleSession *session, JingleSession *update);
		SessionReason handleSessionTermination(JingleSession *sesion);
		SessionReason handleSessionError(JingleSession *session, const gloox::Stanza *stanza);



    protected:
    void addRequest(Request r);
	void removeRequest(Request r);

    private:
    gloox::EchoClient *m_client;
    RequestVector m_requests;
};


#endif

