
#ifndef AUDIO_FILE_PLAYER_H
#define AUDIO_FILE_PLAYER_H

#include <stdint.h>
#include "qpipeline.h"
#include "mediaconfig.h"

/** @brief Class intended as player of audio files using Gstreamer. */
class AudioFilePlayer
{
    public:

    AudioFilePlayer();
    virtual ~AudioFilePlayer();

    /** @brief Called at end of stream. */
    virtual void    streamEnded();
    virtual void    streamError(const std::string &message);
    virtual void    streamWarning(const std::string &message);

    bool setMediaConfig(const MediaConfig &config);
    bool playFile(const char *path);
    bool setFile(const char *path);

    int64_t currentFileDuration();

    void replay();
    void stop();
    void pause();


    MediaDevice fileInput();

    static gboolean messageCallback(GstBus *bus, GstMessage *message, gpointer data);
    
    QPipeline   *pipeline;
};

#endif

