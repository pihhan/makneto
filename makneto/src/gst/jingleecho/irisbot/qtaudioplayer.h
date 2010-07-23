
#ifndef QT_AUDIO_PLAYER_H
#define QT_AUDIO_PLAYER_H

#include <QObject>
#include "audiofileplayer.h"

/** @brief GStreamer based audio file player.
    It needs configuration of output elements from MediaConfig first.
    It can be configured to repeat playing after some interval, with unlimited
    or limited repeats. */
class QtAudioPlayer : public QObject, public AudioFilePlayer
{
    Q_OBJECT
    public:
    QtAudioPlayer();

    /* events from AudioFilePlayer */
    virtual void    streamEnded();
    virtual void    streamError(const std::string &message);
    virtual void    streamWarning(const std::string &message);
    virtual void    streamStarted();

    bool playFile(const QString &path);
    bool setFile(const QString &path);

    void setRepeats(int repeats);
    void setDelay(int delay);

    public slots:
    void repeatTimeout();

    signals:
    /** @brief Emitted when playing of sound ended after last repeat. */
    void finished();
    /** @brief Emitted on pipeline error. Pipeline is unable to continue. */
    void error(const QString &msg);
    /** @brief Emitted on warning pipeline, but playing might be able to continue. */
    void warning(const QString &msg);

    /** @brief Emitted after successful preconfiguration, 
        when file really starts playing. */
    void started();

    private:
    int m_repeats;
    int m_delay;
    
};

#endif
