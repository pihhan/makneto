#ifndef IRISJINGLEMANAGER_H
#define IRISJINGLEMANAGER_H

#include <iris/xmpp_task.h>
#include "jinglemanager.h"

/** @brief Wrap around for Gloox library. 
    This class is implementation of JingleManager using gloox. 
*/
class IrisJingleManager :
    public XMPP::Task,
    public JingleManager
{
    public:
	IrisJingleManager(Task *parent);

        virtual void    send(JingleStanza *js);
        void replyTerminate(const PJid &to, SessionReason reason, const std::string &sid="");
        void    replyAcknowledge(const QDomElement &e);
        void replyError(const QDomElement &e, const QString &errtag=QString(),
            const QString &type="cancel");
        virtual PJid  self();
    
    static QDomElement createJingleStanza(JingleStanza *js, const std::string &id, QDomDocument *doc);
    virtual void commentSession(JingleSession *session, const std::string &comment);
    virtual bool take(const QDomElement &e);

    protected:
        virtual void onGo();
        virtual void onDisconnect();
};

#endif

