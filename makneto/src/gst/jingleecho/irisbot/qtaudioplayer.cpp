
#include <QTimer>

#include "qtaudioplayer.h"

QtAudioPlayer::QtAudioPlayer()
    : m_repeats(1), m_delay(1000)
{
}

/** @brief Handle end of stream by starting timer to run new repeat. */
void QtAudioPlayer::streamEnded()
{
    if (m_repeats > 0) {
        m_repeats--;
        QTimer::singleShot(m_delay, this, SLOT(repeatTimeout()) );
    } else if (m_repeats < 0) {
        // unlimited repeats
        QTimer::singleShot(m_delay, this, SLOT(repeatTimeout()) );
    } else {
        emit finished();
    }
}

void QtAudioPlayer::streamError(const std::string &message)
{
    QString msg = QString::fromStdString(message);
    emit error(msg);
}

void QtAudioPlayer::streamWarning(const std::string &message)
{
    QString msg = QString::fromStdString(message);
    emit warning(msg);
}

/** @brief Emit signal started after message from pipeline. */
void QtAudioPlayer::streamStarted()
{
    emit started();
}

bool QtAudioPlayer::playFile(const QString &path)
{
    return AudioFilePlayer::playFile(path.toLocal8Bit().data());
}

bool QtAudioPlayer::setFile(const QString &path)
{
    return AudioFilePlayer::setFile(path.toLocal8Bit().data());
}

/** @brief Set repeat count for current file. */
void QtAudioPlayer::setRepeats(int repeats)
{
    m_repeats = repeats;
}

/** @brief Set delay between repeats,
    @param delay Delay between end of sample and new start in miliseconds. */
void QtAudioPlayer::setDelay(int delay)
{
    m_delay = delay;
}

void QtAudioPlayer::repeatTimeout()
{
    replay();
}

