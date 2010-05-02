
#include "qtjinglesession.h"
#include "irisjinglemanager.h"

QtJingleSession::QtJingleSession(JingleSession *js, IrisJingleManager *manager)
    : m_session(js), m_manager(manager)
{
}

QtJingleSession::~QtJingleSession()
{
    destroy();
}

JingleSession * QtJingleSession::session()
{
    return m_session;
}

MediaConfig QtJingleSession::config() const
{
    return m_config;
}

void QtJingleSession::setMediaConfig(const MediaConfig &config)
{
    m_config = config;
}

void QtJingleSession::accept() 
{
    m_manager->acceptAudioSession(m_session, m_config);
}

void QtJingleSession::terminate()
{
    m_manager->terminateSession(m_session);
}


void QtJingleSession::reportInfo(SessionInfo info)
{
    switch (info) {
        case INFO_NONE:
            break;
        case INFO_ACTIVE:
            break;
        case INFO_HOLD:
            emit holded();
            break;
        case INFO_MUTE:
            emit muted();
            break;
        case INFO_UNHOLD:
            emit unholded();
            break;
        case INFO_UNMUTE:
            emit unmuted();
            break;
        case INFO_RINGING:
            emit ringing();
            break;
    }
}

void QtJingleSession::reportError(JingleErrors err, const std::string &msg)
{
    emit error(QString::fromStdString(msg));
}

void QtJingleSession::reportFatalError(JingleErrors error, const std::string &msg)
{
    emit fatalError(QString::fromStdString(msg));
}

void QtJingleSession::reportAccepted()
{
    emit accepted();
}

void QtJingleSession::reportTerminate(SessionReason reason)
{
    emit terminated(reason);
}

void QtJingleSession::destroy()
{
    m_manager->removeSession(this);
    m_manager->destroySession(m_session);
}

QString QtJingleSession::qsid() const
{
    return QString::fromStdString(m_session->sid());
}
