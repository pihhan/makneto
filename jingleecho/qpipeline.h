
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

    GstElement *getAudioSource();
    GstElement *getAudioSink();

    GstPad * getAudioSourcePad();
    GstPad * getAudioSinkPad();
    
    bool createFilters();

    std::string describe();
    static std::string binToString(GstElement *bin);

    /* glib callbacks */
    static void elementAdded(GstBin *bin, GstElement *element, gpointer pipeline);
    static void elementRemoved(GstBin *bin, GstElement *element, gpointer pipeline);

    private:

    GstBus *m_bus;
    GstBin *m_bin;
    GstElement *m_pipe;
    GstElement *m_source;
    GstElement *m_sourcefilter;
    GstElement *m_sink;
    GstElement *m_sinkfilter;
    bool m_pausable;
};

#endif // QPIPELINE_H
