
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
    public:
    VersionReceiver();

    virtual void handleVersion(gloox::Stanza *stanza, int context)=0;
    std::string getVerName(gloox::Stanza *stanza);
    std::string getVerNumber(gloox::Stanza *stanza);
    std::string getVerOs(gloox::Stanza *stanza);
};

/** @brief Class handling responses or requests to us. */
class VersionIqHandler : public gloox::IqHandler
{
    public:
    typedef std::vector<VersionReceiver *>  VersionContextVector;

    VersionIqHandler(gloox::ClientBase *base);


    virtual bool handleIq(gloox::Stanza *stanza);
    virtual bool handleIqID(gloox::Stanza *stanza, int context);

    virtual void requestVersion(const gloox::JID &target, VersionReceiver *rcvr, int context=0);

    void setVersion(const std::string &version, const std::string &name, const std::string &os);

    protected:
    void replyVersion(const gloox::Stanza *stanza);

    private:
    gloox::ClientBase  *m_base;
    VersionContextVector m_contexts;
    std::string         m_version;
    std::string         m_name;
    std::string         m_os;
    
};


#endif
