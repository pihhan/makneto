/*
 * Class to help playing file using GStreamer backends. 
 */

#include <string>
#include <iostream>
#include <cstring>
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
    newconfig.setAudioInput(config.fileInput());

    pipeline->setMediaConfig(newconfig);

#if 0
    bool input =  pipeline->enableAudioInput();
    bool output = pipeline->enableAudioOutput();

    if (input && output) {
        return link();
    }

    return input && output;
#else
    return true;
#endif
}

/** @brief Try to link audio input to audio output.
    @return True if link is successfull, false in the other case. 
    If source pad is null, try to find decodebin and connect signal
    to pipeline. */
bool AudioFilePlayer::link()
{
    GstPad * inpad = pipeline->getAudioSourcePad();
    GstPad * outpad = pipeline->getAudioSinkPad();
    bool linked = false;

    if (!inpad && outpad) {
        GstElement *decodebin = pipeline->getElement("audio-source-filter");
        if (!decodebin)
            return false;
        g_signal_connect(decodebin, "new-decoded-pad", 
            G_CALLBACK(QPipeline::onNewDecodedAudioPad), pipeline);
        return true;
    } else {
        linked = pipeline->link(inpad, outpad);
        gst_object_unref(inpad);
    }

    gst_object_unref(outpad);
    return linked;
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
    std::cerr << "AudioFilePlayer" << message << std::endl;
}

void AudioFilePlayer::replay()
{   
    GstElement *source = pipeline->getAudioSource();
    pipeline->setState(GST_STATE_PAUSED);
    if (gst_element_seek_simple(
            source, GST_FORMAT_DEFAULT, GST_SEEK_FLAG_FLUSH, 0)) {
        pipeline->setState(GST_STATE_PLAYING);
    } else {
        std::cerr << "AudioFilePlayer: replay seek failed." << std::endl;
        pipeline->setState(GST_STATE_NULL);
        pipeline->setState(GST_STATE_PLAYING);
    }
}

/** @brief Create audio input from file. */
MediaDevice AudioFilePlayer::fileInput()
{
    MediaDevice device("filesrc");
    device.setFilter("decodebin");
    return device;
}

/** @brief Begin playing of file specified by path.
    @return True if playing seems to started succesfully. */
bool AudioFilePlayer::playFile(const char *path)
{
    if (!pipeline->isAudioEnabled()) {
        bool prepared = prepare();
        if (!prepared || !pipeline->isAudioEnabled())
            return false;
    }

    GstElement *filesrc = pipeline->getAudioSource();
    if (filesrc) {
        gchar *current = NULL;
        g_object_get(G_OBJECT(filesrc), "location", &current, NULL);
        if (!current || strcmp(path, current)) {
            pipeline->setState(GST_STATE_NULL);
            g_object_set(G_OBJECT(filesrc), "location", path, NULL);
        }

        if (current) 
            g_free(current);
        pipeline->setState(GST_STATE_PLAYING);
        return true;
    } else
        return false;
}

/** @brief Set current file without running playing. */
bool AudioFilePlayer::setFile(const char *path)
{
    GstElement *filesrc = pipeline->getAudioSource();
    if (filesrc) {
        gchar *current = NULL;
        g_object_get(G_OBJECT(filesrc), "location", &current, NULL);
        if (!current || strcmp(current, path))
            g_object_set(G_OBJECT(filesrc), "location", path, NULL);
        if (current)
            g_free(current);
        return true;
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
    GstFormat format = GST_FORMAT_TIME;
    GstElement *source = pipeline->getAudioSource();
    gst_element_query_duration(source, &format, &time);
    return time;
}

/** @brief Handle messages from pipeline elements. */
gboolean AudioFilePlayer::messageCallback(GstBus * , GstMessage *message, gpointer data)
{
    AudioFilePlayer *player = static_cast<AudioFilePlayer *> (data);
    switch (message->type) {
        case GST_MESSAGE_EOS:
        case GST_MESSAGE_SEGMENT_DONE:
            player->streamEnded();
            break;

        case GST_MESSAGE_ERROR:
            {
                GError *err = NULL;
                gchar *debug = NULL;
                gst_message_parse_error(message, &err, &debug);
                std::string errmsg(err->message);
                if (debug)
                    errmsg = errmsg + " (" + debug + ")";
                player->streamError(errmsg); 
                g_error_free(err);
                g_free(debug);
            }
            break;
        case GST_MESSAGE_WARNING:
            {
                GError *err = NULL;
                gchar *debug = NULL;
                gst_message_parse_warning(message, &err, &debug);
                std::string errmsg(err->message);
                if (debug)
                    errmsg = errmsg + " (" + debug + ")";
                player->streamError(errmsg);
                g_error_free(err);
                g_free(debug);
            }
            break;
        default:
            break;
    }
    return TRUE;
}

/** @brief Prepare pipeline to be ready for audio files. */
bool AudioFilePlayer::prepare()
{
    bool prepi = true;
    bool prepo = true;
    if (!pipeline->isAudioInputEnabled()) 
        prepi = pipeline->enableAudioInput();
    if (!pipeline->isAudioOutputEnabled())
        prepo = pipeline->enableAudioOutput();
    bool linked = link();
    return prepi && prepo && linked;
}

bool AudioFilePlayer::isReady() 
{
    if (!pipeline->isValid() || !pipeline->isAudioInputEnabled())
        return false;
    GstPad *pad = pipeline->getAudioSourcePad();
    bool linked = gst_pad_is_linked(pad);
    gst_object_unref(pad);

    return (
           pipeline->isAudioInputEnabled() 
        && pipeline->isAudioInputEnabled()
        && linked);
}

