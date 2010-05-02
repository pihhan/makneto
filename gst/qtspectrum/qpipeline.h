
#ifndef QPIPELINE_H
#define QPIPELINE_H

#include "gst/gst.h"


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

    private:

    GstBus *m_bus;
    GstBin *m_bin;
    GstElement *m_pipe;
    bool m_pausable;
};

#endif // QPIPELINE_H
