

#include <gst/gst.h>
#include "qpipeline.h"

#ifdef QT
#include <QDebug>
#define QPLOG() (qDebug())
#else // !QT
#include <iostream>
#include <string>
#include "logger/logger.h"
#define QPLOG() (LOGGER(logit))

void qCritical(const std::string &msg)
{
    logit << msg << std::endl;
}
#endif


QPipeline::QPipeline()
{
    m_pipe = gst_pipeline_new("qpipeline");
    g_assert(m_pipe);
    g_signal_connect(GST_BIN(m_pipe), "element-added", G_CALLBACK(elementAdded), this);
    g_signal_connect(GST_BIN(m_pipe), "element-removed", G_CALLBACK(elementRemoved), this);
    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipe));
    m_pausable = true;

    if (!m_pipe) {
        qCritical("Created pipeline is NULL");
        return;
    }
    if (createAudioSource() && createAudioSink()) {
        add(m_source);
        add(m_sink);
    } else {
        QPLOG() << "creating sink our source failed" << std::endl;
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
        case GST_STATE_CHANGE_ASYNC:
            break;
        case GST_STATE_CHANGE_FAILURE:
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

GstState QPipeline::pending_state()
{
    GstState current;
    GstState pending;

    GstStateChangeReturn ret = gst_element_get_state(m_pipe, &current, &pending, 0);
    if (ret != GST_STATE_CHANGE_FAILURE) {
        return pending;
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

bool QPipeline::createAudioSource()
{
    const gchar *env = g_getenv("AUDIOSOURCE");
    GError *err = NULL;
    if (env) {
        m_source = gst_parse_bin_from_description(env, TRUE, &err);
    } else {
        m_source = gst_parse_bin_from_description(DEFAULT_AUDIOSOURCE, TRUE, &err);
    }
    if (err) {
        QPLOG() << " gst_parse_bin_from_description: " <<
            err->message << std::endl;
    }
    g_assert(m_source);
    return (m_source != NULL);
}

bool QPipeline::createAudioSink()
{
    const gchar *env = g_getenv("AUDIOSINK");
    GError *err = NULL;
    if (env) {
        m_sink = gst_parse_bin_from_description(env, TRUE, &err);
    } else {
        m_sink = gst_parse_bin_from_description(DEFAULT_AUDIOSINK, TRUE, &err);
    }
    if (err) {
        QPLOG() << " gst_parse_bin_from_description: " <<
            err->message << std::endl;
    }
    g_assert(m_sink);
    return (m_sink != NULL);
}

GstElement *QPipeline::getAudioSource()
{
    return m_source;
}

GstElement *QPipeline::getAudioSink()
{
    return m_sink;
}

GstPad * QPipeline::getAudioSourcePad()
{
    GstPad *pad = gst_element_get_static_pad(m_source, "src");
    return pad;
}

GstPad * QPipeline::getAudioSinkPad()
{
    GstPad *pad = gst_element_get_static_pad(m_sink, "sink");
    return pad;
}

void QPipeline::elementAdded(GstBin *bin, GstElement *element, gpointer pipeline)
{
    QPLOG() << "added element: " << gst_element_get_name(element) << std::endl;
}

void QPipeline::elementRemoved(GstBin *bin, GstElement *element, gpointer pipeline)
{
    QPLOG() << "removed element: " << gst_element_get_name(element) << std::endl;
}

void QPipeline::describe()
{
    GstIterator *it;
    bool done = FALSE;
    GstElement *element;

    it = gst_bin_iterate_elements(GST_BIN(m_pipe));
    QPLOG() << "Elements in pipeline " << gst_element_get_name(m_pipe) << std::endl;
    while(!done) {
        switch (gst_iterator_next(it, (void **) &element)) {
            case GST_ITERATOR_OK:
                QPLOG() << gst_element_get_name(element) << std::endl;
                gst_object_unref(element);
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            case GST_ITERATOR_ERROR:
                QPLOG() << "Chyba iteratoru" << std::endl;
                done = TRUE;
                break;
            case GST_ITERATOR_DONE:
                done = TRUE;
                break;
        }
    }
    gst_iterator_free(it);
}
