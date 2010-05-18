
#ifndef QT_AUDIO_PLAYER_H
#define QT_AUDIO_PLAYER_H

/** @brief GStreamer based audio file player.
    It needs configuration of output elements from MediaConfig first. */
class QtAudioPlayer : public QObject
{
    Q_OBJECT
    public:
    QtAudioPlayer();

    virtual void    streamEnded();
    virtual void    streamError(const std::string &message);
    virtual void    streamWarning(const std::string &message);

    bool playFile(const QString &path);
    bool setFile(const QString &path);

    void setRepeats(int repeats);
    void setDelay(int delay);

    public slots:
    void repeatTimeout();

    signals:
    void finished();
    void error(const QString &msg);
    void warning(const QString &msg);

    private:
    int m_repeats;
    int m_delay;
    
};

#endif
