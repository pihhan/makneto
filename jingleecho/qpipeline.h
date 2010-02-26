
#ifndef QPIPELINE_H
#define QPIPELINE_H

#include <gst/gst.h>
#include <string>

#define DEFAULT_AUDIOSOURCE "audiotestsrc is-live=1 ! audio/x-raw-int,rate=8000 ! audioconvert "
#define DEFAULT_AUDIOSINK   "audioconvert ! pulsesink"

#define DEFAULT_AUDIOSOURCE_FILTER "audioconvert ! audioresample"
#define DEFAULT_AUDIOSINK_FILTER    "audioconvert ! audioresample"

#define DEFAULT_VIDEOSOURCE "v4l2src"
#define DEFAULT_VIDEOSOURCE_FILTER  "capsfilter caps=video/x-raw-yuv,width=640 ! ffmpegcolorspace ! videoscale"

#define DEFAULT_VIDEOSINK   "xvimagesink"
#define DEFAULT_VIDEOSINK_FILTER "ffmpegcolorspace ! videoscale"

/** @brief Simple C++ wrapper for Gstreamer pipeline. */
class QPipeline
{
    public:
    QPipeline();
    virtual ~QPipeline();

    bool add(GstElement *element);

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

    bool createVideoSource(const char *bindesc=NULL, const char *filter=NULL);
    bool createVideoSink(const char *bindesc=NULL, const char *filter=NULL);
    bool createLocalVideoSink(const char *bindesc=NULL, const char *filter=NULL);
    bool createVideoTee();

    bool enableVideo(bool input = true, bool output = true);
    bool enableAudio();

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

    private:

    GstBus *m_bus;
    GstBin *m_bin;
    GstElement *m_pipe;
    GstElement *m_source;
    GstElement *m_sourcefilter;
    GstElement *m_sink;
    GstElement *m_sinkfilter;
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
};

#endif // QPIPELINE_H
