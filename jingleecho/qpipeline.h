
#ifndef QPIPELINE_H
#define QPIPELINE_H

#include <gst/gst.h>
#include <string>

#define DEFAULT_AUDIOSOURCE "audiotestsrc is-live=1 ! audio/x-raw-int,rate=8000 ! audioconvert "
#define DEFAULT_AUDIOSINK   "audioconvert ! pulsesink"

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

    bool createAudioSource();
    bool createAudioSink();

    bool createVideoSource();
    bool createVideoSink();

    GstElement *getAudioSource();
    GstElement *getAudioSink();

    GstPad * getAudioSourcePad();
    GstPad * getAudioSinkPad();

    GstPad * getVideoSourcePad();
    GstPad * getVideoSinkPad();
    
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
    GstElement *m_videoinputtee;
    bool m_pausable;
    bool m_valid;
};

#endif // QPIPELINE_H
