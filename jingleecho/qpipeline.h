
#ifndef QPIPELINE_H
#define QPIPELINE_H

#include <gst/gst.h>

#define DEFAULT_AUDIOSOURCE "audiotestsrc"
#define DEFAULT_AUDIOSINK   "fakesink"

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

    private:

    GstBus *m_bus;
    GstBin *m_bin;
    GstElement *m_pipe;
    GstElement *m_source;
    GstElement *m_sink;
    bool m_pausable;
};

#endif // QPIPELINE_H
