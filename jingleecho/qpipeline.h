
#ifndef QPIPELINE_H
#define QPIPELINE_H

#include <gst/gst.h>
#include <string>

#include "mediaconfig.h"


/** @brief Simple C++ wrapper for Gstreamer pipeline. */
class QPipeline
{
    public:
    QPipeline();
    virtual ~QPipeline();

    bool add(GstElement *element);
    bool remove(GstElement *element);

    GstBus *bus();
    GstElement *element();

    bool setState(GstState newstate);
    GstState current_state();
    GstState pending_state();

    /** @brief Link source to destination. 
        Both of them have to be already added inside this pipeline.
    */
    bool link(GstElement *source, GstElement *destination);
    bool link(GstPad *srcpad, GstPad *dstpad);
    bool link(GstElement *src, GstPad *dstpad);
    bool link(GstPad *srcpad, GstElement *dst);

    bool createAudioSource();
    bool createAudioSink();

    GstElement *getAudioSource();
    GstElement *getAudioSink();

    bool createVideoSource();
    bool createVideoSink();
    bool createLocalVideoSink();
    bool createVideoTee();

    bool enableVideo(bool input = true, bool output = true);
    bool enableAudio();

    bool enableLocalVideoSink();
    bool disableLocalVideoSink();
    bool enableVideoInput();

    bool isVideoEnabled();
    bool isAudioEnabled();

    GstPad * getAudioSourcePad();
    GstPad * getAudioSinkPad();

    GstPad * getVideoSourcePad();
    GstPad * getVideoSinkPad();
    
    GstElement * getVideoSource();
    GstElement * getVideoSink();
    GstElement * getLocalVideoSink();

    bool createFilters();

    std::string describe();
    static std::string binToString(GstElement *bin);

    /* glib callbacks */
    static void elementAdded(GstBin *bin, GstElement *element, gpointer pipeline);
    static void elementRemoved(GstBin *bin, GstElement *element, gpointer pipeline);

    /** @brief Check whether pipeline was created and no fatal error occured. */
    bool    isValid();

    MediaConfig mediaConfig();
    void setMediaConfig(const MediaConfig &c);
    bool configureElement(GstElement *e, PayloadParameterMap p);

    private:

    GstBus *m_bus;
    GstBin *m_bin;
    GstElement *m_pipe;
    GstElement *m_asource;
    GstElement *m_asourcefilter;
    GstElement *m_asink;
    GstElement *m_asinkfilter;
    GstElement *m_videosource;
    GstElement *m_vsourcefilter;
    GstElement *m_videosink;
    GstElement *m_vsinkfilter;
    GstElement *m_localvideosink;
    GstElement *m_lvsinkfilter;
    GstElement *m_videoinputtee;
    bool m_pausable;
    bool m_valid;
    bool m_video_enabled;
    bool m_audio_enabled;
    MediaConfig m_config;
};

#endif // QPIPELINE_H
