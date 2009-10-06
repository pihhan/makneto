
#ifndef VERSIONHANDLER_H
#define VERSIONHANDLER_H

#include <vector>

#include <gloox/iqhandler.h>
#include <gloox/jid.h>
#include <gloox/clientbase.h>


/** @brief This class acts as interface to be reimplemented by
    real handling method. */
class VersionReceiver
{
    VersionReceiver();

    virtual void handleVersion(Stanza *stanza, int context)=0;
    std::string getVerName(Stanza *stanza);
    std::string getVerNumber(Stanza *stanza);
    std::string getVerOs(Stanza *stanza);
};

/** @brief Class handling responses or requests to us. */
class VersionIqHandler : public gloox::IqHandler
{
    public:
    typedef std::vector<VersionReceiver *>  VersionContextVector;

    VersionIqHandler(ClientBase *base);


    virtual bool handleIq(gloox::Stanza *stanza);
    virtual bool handleIqID(gloox::Stanza *stanza, int context);

    virtual void requestVersion(const gloox::JID &target, VersionReceiver *rcvr, int context=0);

    void setVersion(const std::string &version, const std::string &name, const std::string &os);

    protected:
    void replyVersion(Stanza *stanza);

    private:
    gloox::ClientBase  *m_client;
    VersionContextVector m_contexts;
    std::string         m_version;
    std::string         m_name;
    std::string         m_os;
    
};


#endif
