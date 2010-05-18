/*
 * Class to help playing file using GStreamer backends. 
 */

#include <string>
#include "audiofileplayer.h"

AudioFilePlayer::AudioFilePlayer()
{
    pipeline = new QPipeline();

    GstBus *bus = pipeline->bus();   
    gst_bus_add_watch(bus, messageCallback, this);
    gst_object_unref(GST_OBJECT(bus));
}

AudioFilePlayer::~AudioFilePlayer()
{
    delete pipeline;
}

/** @brief Configure config for ring output. 
    Only ringOutput will be used as audioOutput. */
bool AudioFilePlayer::setMediaConfig(const MediaConfig &config)
{
    MediaConfig newconfig(config);
    newconfig.setAudioOutput(config.ringOutput());
    newconfig.setAudioInput(fileInput());

    pipeline->setMediaConfig(newconfig);

    return  pipeline->enableAudioInput() &&
            pipeline->enableAudioOutput();
}

/** @brief Called at end of stream. */
void AudioFilePlayer::streamEnded()
{
    // todo: reschedule new playing
}

void AudioFilePlayer::streamError(const std::string &message)
{
    std::cerr << "AudioFilePlayer" << message << std::endl;
}

void AudioFilePlayer::streamWarning(const std::string &message)
{
    std::cerr << "AudioFilePlayer" << message << std::endl;
}

void AudioFilePlayer::replay()
{   
    GstElement *source = pipeline->getAudioSource();
    if (gst_element_seek_simple(source, GST_FORMAT_PERCENT, 0, 0)) {
        pipeline->setState(GST_STATE_PLAYING);
    }
}

/** @brief Create audio input from file. */
MediaDevice AudioFilePlayer::fileInput()
{
    MediaDevice device("filesrc");
    device.setFilter("decodebin");
}

/** @brief Begin playing of file specified by path.
    @return True if playing seems to started succesfully. */
bool AudioFilePlayer::playFile(const char *path)
{
    GstElement *filesrc = pipeline->getAudioSource();
    if (filesrc) {
        bool success = g_object_set(G_OBJECT(filesrc), "location", path, NULL);
        pipeline->setState(GST_STATE_PLAYING);
        return success;
    } else
        return false;
}

/** @brief Set current file without running playing. */
bool AudioFilePlayer::setFile(const char *path)
{
    GstElement *filesrc = pipeline->getAudioSource();
    if (filesrc) {
        bool success = g_object_set(G_OBJECT(filesrc), "location", path, NULL);
        return success;
    } else
        return false;
}

void AudioFilePlayer::stop()
{
    pipeline->setState(GST_STATE_NULL);
}

void AudioFilePlayer::pause()
{
    pipeline->setState(GST_STATE_PAUSED);
}

/** @brief Get current file length in nanoseconds. */
int64_t AudioFilePlayer::currentFileDuration()
{
    int64_t time = -1;
    GstElement *source = pipeline->getAudioSource();
    gst_element_query_duration(source, GST_FORMAT_TIME, &time);
    return time;
}

/** @brief Handle messages from pipeline elements. */
void AudioFilePlayer::messageCallback(GstBus *bus, GstMessage *message, gpointer data)
{
    AudioFilePlayer *player = static_cast<AudioFilePlayer *> (data);
    switch (message->type) {
        case GST_MESSAGE_EOS:
        case GST_MESSAGE_SEGMENT_DONE:
            streamEnded();
            break;

        case GST_MESSAGE_ERROR:
            {
                GError *err = NULL;
                gchar *debug = NULL;
                gst_message_parse_error(message, &err, &debug);
                
                g_error_free(err);
                g_free(debug);
            }
            break;
        case GST_MESSAGE_WARNING:
            {
                GError *err = NULL;
                gchar *debug = NULL;
                gst_message_parse_warning(message, &err, &debug);
                
                g_error_free(err);
                g_free(debug);
            }
            break;
    }
}

