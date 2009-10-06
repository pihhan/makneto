
#ifndef REQUESTS_H
#define REQUESTS_H

/** @brief Store one request and who initiated it. */
class Request
{
    public:
    typedef enum {
        NONE,
        DISCO_ITEMS,
        DISCO_INFO,
        VCARD,
        VERSION
    } RequestType;

    Request();

    gloox::JID  from;
    gloox::JID  to;
    int         context;
    RequestType type;
};

class RequestList : public gloox::DiscoHandler
{
    public:
    typedef std::list<Request>  RequestList;
    typedef std::vector<Request> RequestVector;
    const size_t    vectorSize = 1000;

    RequestList(EchoClient *client);

    virtual void handleDiscoInfoResult(Stanza *stanza, int context);
    virtual void handleDiscoItemsResult(Stanza *stanza, int context);
    virtual void handleDiscoError(Stanza *stanza, int context);
    virtual void handleDiscoSet(Stanza *stanza);

    int generateContext();
    Request getRequest(int context);
    Request createRequest(Request::RequestType t);
    void createDiscoRequest(gloox::JID origin, gloox::JID target, const std::string &node = std::string());

    protected:
    void addRequest(Request r);

    private:
    EchoClient *m_client;
    RequestVector m_requests;
};


#endif

