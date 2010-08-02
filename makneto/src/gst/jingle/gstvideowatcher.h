
#ifndef GSTVIDEOWATCHER_H
#define GSTVIDEOWATCHER_H

#include "framesize.h"

class AVOutput;

/** @brief Gstreamer interface to GUI and back. */
class GstVideoWatcher
{
    public:
    GstVideoWatcher();
    virtual ~GstVideoWatcher();

    // request from gstreamer to gui
    virtual void prepareWindowId() = 0;
    virtual void videoResolutionChanged(const FrameSize &size) = 0;

    // request from gui to gstreamer
    virtual void setWindowId(unsigned long id) = 0;
    virtual void handleExpose() = 0;

    void setOutput(AVOutput *out);
    void setVideoOutput(AVOutput *out);
    AVOutput * output() const;
    AVOutput * videoOutput() const;

    private:
    AVOutput    *m_output;

};

#endif
