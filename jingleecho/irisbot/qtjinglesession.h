
#ifndef QTJINGLESESSION_H
#define QTJINGLESESSION_H

#include <QObject>
#include "jinglesession.h"
#include "mediaconfig.h"
#include "jingleerror.h"

class IrisJingleManager;

/** @brief Qt version of JingleSession, intended for interaction with GUI
    written in C++ using Qt library. */
class QtJingleSession : public QObject
{
    Q_OBJECT
    public:

    QtJingleSession(JingleSession *js, IrisJingleManager *manager);
    virtual ~QtJingleSession();

    JingleSession *session();

    MediaConfig config() const;
    QString qsid() const;
    void setMediaConfig(const MediaConfig &config);

    void reportInfo(SessionInfo info);
    void reportError(JingleErrors error, const std::string &msg);
    void reportFatalError(JingleErrors error, const std::string &msg);
    void reportAccepted();
    void reportTerminate(SessionReason reason);

    public slots:
    void accept();
    void terminate();
    void destroy();

    signals:
    void failed();
    void accepted();
    void refused(SessionReason reason);
    void running();
    void terminated(SessionReason reason);
    void timedOut();

    // SessionInfo signals
    void active();
    void ringing();
    void muted();
    void holded();
    void unmuted();
    void unholded();

    void error(const QString &msg);
    void info(const QString &msg);
    void fatalError(const QString &msg); 

    private:
    JingleSession   *m_session;
    IrisJingleManager *m_manager;
    MediaConfig     m_config;
};

#endif

