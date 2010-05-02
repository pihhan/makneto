
#include <gst/gst.h>
#include <QDebug>

#include "qpipeline.h"

QPipeline::QPipeline()
{
    m_pipe = gst_pipeline_new("qpipeline");
    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipe));
    m_pausable = true;

    if (!m_pipe) {
        qCritical("Created pipeline is NULL");
        return;
    }

}

QPipeline::~QPipeline()
{
    gst_object_unref(m_bus);
    gst_object_unref(m_pipe);
}

bool QPipeline::add(GstElement *element)
{
    return gst_bin_add(GST_BIN(m_pipe), element);
}

bool QPipeline::link(GstElement *source, GstElement *destination)
{
    return gst_element_link(source, destination);
}

bool QPipeline::setState(GstState newstate)
{
    GstStateChangeReturn ret = gst_element_set_state(m_pipe, newstate);
    switch (ret) {
        case GST_STATE_CHANGE_SUCCESS:
            m_pausable = true;
            break;
        case GST_STATE_CHANGE_NO_PREROLL:
            m_pausable = false;
            break;
        default:
            break;
    }
    return (ret != GST_STATE_CHANGE_FAILURE);
}

/** @brief Get current status of pipeline.
    If there is problem, returns GST_STATE_VOID_PENDING as sign of error. */
GstState QPipeline::current_state()
{
    GstState current;
    GstState pending;

    GstStateChangeReturn ret = gst_element_get_state(m_pipe, &current, &pending, 0);
    if (ret != GST_STATE_CHANGE_FAILURE) {
        return current;
    } else {
        return GST_STATE_VOID_PENDING;
    }
}

GstElement * QPipeline::element()
{
    return m_pipe;
}

/** @brief Get current bus.
    For now, do NOT unlink that bus after you are done with it, it is internal. 
*/
GstBus * QPipeline::bus()
{
    return m_bus;
}
