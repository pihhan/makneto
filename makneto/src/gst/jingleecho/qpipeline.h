
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
    bool enableAudioInput();
    bool disableAudioInput();
    bool enableAudioOutput();
    bool disableAudioOutput();

    bool enableLocalVideoSink();
    bool disableLocalVideoSink();
    bool enableVideoInput();
    bool disableVideoInput();


    GstPad * getAudioSourcePad();
    GstPad * getAudioSinkPad();

    GstPad * getVideoSourcePad();
    GstPad * getVideoSinkPad();
    
    GstElement * getVideoSource();
    GstElement * getVideoSink();
    GstElement * getLocalVideoSink();

    /** @brief Get element from pipeline by its name. */
    GstElement * getElement(const char *name);

    bool createFilters();

    std::string describe();
    static std::string binToString(GstElement *bin);

    /* glib callbacks */
    static void elementAdded(GstBin *bin, GstElement *element, gpointer pipeline);
    static void elementRemoved(GstBin *bin, GstElement *element, gpointer pipeline);
    static void onNewDecodedAudioPad(GstElement *object,
        GstPad *src, gboolean arg1, gpointer pipe_data);


    bool isVideoEnabled();
    bool isAudioEnabled();

    bool isAudioInputEnabled();
    bool isAudioOutputEnabled();
    bool isVideoInputEnabled();
    bool isVideoOutputEnabled();

    /** @brief Check whether pipeline was created and no fatal error occured. */
    bool    isValid();

    MediaConfig mediaConfig();
    void setMediaConfig(const MediaConfig &c);
    static bool configureElement(GstElement *e, PayloadParameterMap p);

    static std::string describePad(GstPad *pad, bool display_peer=true);

    protected:
    static bool createElementsFromDevice(
        const MediaDevice &d,
        GstElement **source,
        GstElement **filterelem,
        const std::string &name);

    private:

    GstBus *m_bus;
    GstBin *m_bin;
    GstElement *m_pipe;
    GstElement *m_asource;
    GstElement *m_asourcefilter;
    GstElement *m_asink;
    GstElement *m_asinkmixer;
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
    bool m_audioInputEnabled;
    bool m_videoInputEnabled;
    bool m_audioOutputEnabled;
    bool m_videoOutputEnabled;
    MediaConfig m_config;
};

#endif // QPIPELINE_H
