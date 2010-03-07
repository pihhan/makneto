
#ifndef AVOUTPUT_H
#define AVOUTPUT_H

#include <string>
#include <gst/gst.h>
#include "mediaconfig.h"

class QPipeline;

/** @brief Class for output of one remote person, with audio and video elements.
    This can create audio and video outputs, configure them. */
class AVOutput
{
    public:
    AVOutput(QPipeline *p);
    AVOutput(QPipeline *p, MediaDevice audio, MediaDevice video);

    virtual ~AVOutput();

    bool enableAudio();
    bool enableVideo();

    bool audioEnabled();
    bool videoEnabled();

    MediaDevice videoConfig() const;
    MediaDevice audioConfig() const;

    void setVideoConfig(const MediaDevice &d);
    void setAudioConfig(const MediaDevice &d);

    GstPad *getVideoSinkPad();
    GstPad *getAudioSinkPad();

    GstElement *videoElement();
    GstElement *audioElement();
    GstElement *videoFilterElement();
    GstElement *audioFilterElement();

    std::string name();
    void setName(const std::string &name);

    private:
    std::string participant;
    GstElement *m_vsink;
    GstElement *m_asink;
    GstElement *m_afilter;
    GstElement *m_vfilter;
    MediaDevice m_audioconfig;
    MediaDevice m_videoconfig;
    QPipeline   *m_pipeline;
};

#endif

