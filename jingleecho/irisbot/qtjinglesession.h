
#ifndef QTJINGLESESSION_H
#define QTJINGLESESSION_H

#include <QObject>
#include "jinglesession.h"

/** @brief Qt version of JingleSession, intended for interaction with GUI
    written in C++ using Qt library. */
class QtJingleSession
{
    Q_OBJECT
    public:

    QtJingleSession(JingleSession *js);

    JingleSession *session();

    signals:
    void failed();
    void accepted();
    void refused(SessionReason reason);
    void running();

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
    JingleSession *m_session;
};

#endif

