
#ifndef AVOUTPUT_H
#define AVOUTPUT_H

#include <string>
#include <gst/gst.h>
#include "mediaconfig.h"

class QPipeline;
class GstAudioWatcher;
class GstVideoWatcher;

namespace farsight {
    class Stream;
}

/** @brief Class for output of one remote person, with audio and video elements.
    This can create audio and video outputs, configure them. */
class AVOutput
{
    public:
    AVOutput(QPipeline *p);
    AVOutput(QPipeline *p, MediaDevice audio, MediaDevice video);

    typedef unsigned long   WindowId;

    virtual ~AVOutput();

    virtual bool enableAudio();
    virtual bool enableVideo();

    virtual bool disableAudio();
    virtual bool disableVideo();

    virtual bool audioEnabled();
    virtual bool videoEnabled();

    MediaDevice videoConfig() const;
    MediaDevice audioConfig() const;

    void setVideoConfig(const MediaDevice &d);
    void setAudioConfig(const MediaDevice &d);

    virtual GstPad * getVideoSinkPad();
    virtual GstPad * getAudioSinkPad();

    GstElement *videoElement();
    GstElement *audioElement();
    GstElement *videoFilterElement();
    GstElement *audioFilterElement();
    GstElement *levelElement();
    GstElement *volumeElement();

    std::string name();
    void setName(const std::string &name);

    bool handleLevelMessage(GstMessage *msg);
    void parseLevelMessage(GstMessage *msg);

    void registerAudioWatcher(GstAudioWatcher *watcher);
    void registerVideoWatcher(GstVideoWatcher *watcher);

    bool createAudioSink();
    bool createVideoSink();

    virtual void expose();
    virtual void setWindowId(unsigned long id);

    virtual double volume() const;
    virtual void setVolume(double volume);

    virtual bool muted() const;
    virtual void setMuted(bool muted);


    farsight::Stream *audioStream() const;
    farsight::Stream *videoStream() const;
    void setAudioStream(farsight::Stream *s);
    void setVideoStream(farsight::Stream *s);

    private:
    std::string participant;
    protected:
    GstElement *m_vsink;
    GstElement *m_asink;
    GstElement *m_afilter;
    GstElement *m_vfilter;
    GstElement *m_level; ///<! level element, to read audio volume levels
    GstElement *m_volume; ///<! volume element, to modify audio volume
    private:
    MediaDevice m_audioconfig;
    MediaDevice m_videoconfig;
    protected:
    QPipeline   *m_pipeline;
    GstAudioWatcher *m_audiowatcher;
    GstVideoWatcher *m_videowatcher;
    farsight::Stream      *m_videoStream;
    farsight::Stream      *m_audioStream;
};

#endif

