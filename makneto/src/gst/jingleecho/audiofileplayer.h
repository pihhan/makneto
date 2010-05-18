
#ifndef AUDIO_FILE_PLAYER_H
#define AUDIO_FILE_PLAYER_H

#include "qpipeline.h"
#include "mediaconfig.h"

/** @brief Class intended as player of audio files using Gstreamer. */
class AudioFilePlayer
{
    public:

    AudioFilePlayer();

    /** @brief Called at end of stream. */
    virtual void    streamEnded();
    virtual void    streamError(const std::string &message);
    virtual void    streamWarning(const std::string &message);

    bool setMediaConfig(const MediaConfig &config);
    bool playFile(const char *path);
    bool setFile(const char *path);

    void replay();
    void stop();
    void pause();


    MediaDevice fileInput();

    static void messageCallback(GstBus *bus, GstMessage *message, gpointer data);
    
    QPipeline   *pipeline;
};

#endif

