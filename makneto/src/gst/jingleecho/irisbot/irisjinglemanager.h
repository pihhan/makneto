#ifndef IRISJINGLEMANAGER_H
#define IRISJINGLEMANAGER_H

#include <QMap>
#include <QString>
#include <string>

#include <xmpp_task.h>
#include "jinglemanager.h"

class QtJingleSession;

/** @brief Wrap around for Iris library. 
    This class is implementation of JingleManager using iris library. 
*/
class IrisJingleManager :
    public XMPP::Task,
    public JingleManager
{
    Q_OBJECT
    public:

    typedef QMap<QString, QtJingleSession *>    QtJingleSessionMap;

    IrisJingleManager(Task *parent);

    virtual void    send(JingleStanza *js);
    void replyTerminate(const PJid &to, SessionReason reason, const std::string &sid="");
    void replyAcknowledge(const QDomElement &e);
    void replyError(const QDomElement &e, const QString &errtag=QString(),
        const QString &type="cancel");
    virtual PJid  self();
    
    static QDomElement createJingleStanza(JingleStanza *js, const std::string &id, QDomDocument *doc);
    virtual void commentSession(JingleSession *session, const std::string &comment);
    virtual bool take(const QDomElement &e);
    virtual void setState(JingleSession *session, SessionState state);
    virtual void reportInfo(JingleSession *session, SessionInfo info);
    virtual void reportError(JingleSession *session, JingleErrors error, const std::string &msg);
    virtual void reportFatalError(JingleSession *session, JingleErrors error, const std::string &msg);

    QtJingleSession *getQtSession(const QString &sid);
    QtJingleSession *getQtSession(const std::string &sid);
    QtJingleSession *getQtSession(JingleSession *js);

    void addSession(QtJingleSession *qjs);
    void removeSession(QtJingleSession *qjs);

    public slots: 
    /** @brief Accept incoming session. */
    void sessionAccept(JingleSession *session);

    signals:
    /** @brief Emitted on change of existing session. */
    void sessionStateChanged(JingleSession *session);
    /** @brief Emitted on incoming session-initiate. */
    void sessionIncoming(JingleSession *session);
    /** @brief Emitted on end of session. */
    void sessionTerminated(JingleSession *session);
    /** @brief Emitted on incoming session info, like ringing, hold, mute. */
    void sessionInfo(JingleSession *session, SessionInfo info);

    void sessionIncoming(QtJingleSession *session);

    protected:
        virtual void onGo();
        virtual void onDisconnect();

    private:
    QtJingleSessionMap  m_qtsessions;
};

#endif

