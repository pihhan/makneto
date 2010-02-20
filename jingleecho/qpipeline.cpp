
#include <sstream>
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
    : m_source(0), m_sourcefilter(0), m_sink(0), m_sinkfilter(0), 
      m_valid(false), m_video_enabled(false), m_audio_enabled(false)
{
    m_pipe = gst_pipeline_new("qpipeline");
    if (!m_pipe) {
        QPLOG() << "Gstreamer pipeline creation failed." << std::endl;
        m_valid = false;
        return;
    }
    g_signal_connect(GST_BIN(m_pipe), "element-added", G_CALLBACK(elementAdded), this);
    g_signal_connect(GST_BIN(m_pipe), "element-removed", G_CALLBACK(elementRemoved), this);
    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipe));
    m_pausable = true;

    m_valid = true;
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

bool QPipeline::link(GstPad *srcpad, GstPad *dstpad)
{
    GstPadLinkReturn r;
    r= gst_pad_link(srcpad, dstpad);
    switch (r) {
        case GST_PAD_LINK_OK:
        case GST_PAD_LINK_WAS_LINKED:
            return true;
        case GST_PAD_LINK_WRONG_HIERARCHY:
        case GST_PAD_LINK_WRONG_DIRECTION:
        case GST_PAD_LINK_NOFORMAT:
        case GST_PAD_LINK_NOSCHED:
        case GST_PAD_LINK_REFUSED:
            return false;
    }
    return false;
}

bool QPipeline::link(GstElement *src, GstPad *dstpad)
{
    GstElement *dstelement = gst_pad_get_parent_element(dstpad);
    if (dstelement) {
        gchar *name = gst_pad_get_name(dstpad);
        gboolean r = gst_element_link_pads(src, NULL, dstelement, name);
        g_free(name);
        gst_object_unref(dstelement);
        return r;
    } else {
        return false;
    }
}

bool QPipeline::link(GstPad *srcpad, GstElement *dst)
{
    GstElement *srcelement = gst_pad_get_parent_element(srcpad);
    if (srcelement) {
        gchar *name = gst_pad_get_name(srcpad);
        gboolean r = gst_element_link_pads(srcelement, name, dst, NULL);
        g_free(name);
        gst_object_unref(srcelement);
        return r;
    } else {
        return false;
    }
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
    gst_object_ref(m_pipe);
    return m_pipe;
}

/** @brief Get current bus.
    For now, do NOT unlink that bus after you are done with it, it is internal. 
*/
GstBus * QPipeline::bus()
{
    gst_object_ref(m_bus);
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
    if (!m_source && err) {
        QPLOG() << " gst_parse_bin_from_description: " <<
            err->message << std::endl;
    }
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
    if (!m_sink && err) {
        QPLOG() << " gst_parse_bin_from_description: " <<
            err->message << std::endl;
    }
    return (m_sink != NULL);
}

bool QPipeline::createVideoSource(const char *bindesc, const char *filter)
{
    GError *err = NULL;
    const gchar *env = g_getenv("VIDEOSOURCE");
    if (bindesc) {
        m_videosource = gst_parse_bin_from_description(bindesc, TRUE, &err);
    } else if (env) {
        m_videosource = gst_parse_bin_from_description(env, TRUE, &err);
    } else {
        m_videosource = gst_parse_bin_from_description(DEFAULT_VIDEOSOURCE, TRUE, &err);
    }
    if (err) {
        QPLOG() << "vytvareni video zdroje selhalo: " << err->message << std::endl;
        g_error_free(err);
        err = NULL;
    }

    if (filter) {
        m_vsourcefilter = gst_parse_bin_from_description(filter, TRUE, &err);
    } else {
        m_vsourcefilter = gst_parse_bin_from_description(DEFAULT_VIDEOSOURCE_FILTER, TRUE, &err);
    }
    if (err) {
        QPLOG() << "vytvareni video filtru zdroje selhalo: " << err->message << std::endl;
    }


    return (m_videosource != NULL);
}

/** @brief Create output window for displaying of remote content, and filter it
    will use.
*/
bool QPipeline::createVideoSink(const char *bindesc, const char *filter)
{
    GError *err = NULL;
    const gchar *element = bindesc;
    if (!element) {
        element = g_getenv("VIDEOSINK");
        if (!element) {
            element = DEFAULT_VIDEOSINK;
        }
    }
    m_videosink = gst_element_factory_make(element, "remote-video-sink");
    if (m_videosink) {
        QPLOG() << "vytvareni video vystupu selhalo." << std::endl;
    }
    if (filter) {
        m_vsinkfilter = gst_parse_bin_from_description(filter, TRUE, &err);
    } else if (DEFAULT_VIDEOSINK_FILTER) {
        m_vsinkfilter = gst_parse_bin_from_description(DEFAULT_VIDEOSINK_FILTER, TRUE, &err);
    }
    if (err) {
        QPLOG() << "vytvareni filtru video vystupu selhalo: " << err->message << std::endl;
    }
    return (m_videosink != NULL);
}

/** @brief Create element for local video input loopback, as preview.
*/
bool QPipeline::createLocalVideoSink(const char *bindesc, const char *filter)
{
    GError *err = NULL;
    const gchar *element = bindesc;
    if (!element) {
        element = g_getenv("VIDEOSINK");
        if (!element) {
            element = DEFAULT_VIDEOSINK;
        }
    }
    m_localvideosink = gst_element_factory_make(element, "local-video-sink");
    if (!m_localvideosink) {
        QPLOG() << "vytvareni video vystupu selhalo. " << std::endl;
    }
    if (filter) {
        m_lvsinkfilter = gst_parse_bin_from_description(filter, TRUE, &err);
    } else if (DEFAULT_VIDEOSINK_FILTER) {
        m_lvsinkfilter = gst_parse_bin_from_description(DEFAULT_VIDEOSINK_FILTER, TRUE, &err);
    }
    if (err) {
        QPLOG() << "vytvareni filtru video vystupu selhalo: " << err->message << std::endl;
    }
    return (m_localvideosink != NULL);
}

/** @brief Create element that will split input to network and local preview. */
bool QPipeline::createVideoTee()
{
    m_videoinputtee = gst_element_factory_make("tee", "video-source-tee");
    return (m_videoinputtee != NULL);
}

/** @brief Create elements for video input and output. */
bool QPipeline::enableVideo(bool input, bool output)
{
    if (input) {
        if (createVideoSource() && createVideoTee()) {
            add(m_videosource);
            add(m_videoinputtee);
            if (m_vsourcefilter) {
                add(m_vsourcefilter);
                link(m_videosource, m_vsourcefilter);
                link(m_vsourcefilter, m_videoinputtee);
            } else {
                link(m_videosource, m_videoinputtee);
            }
        }
    }
    
    if (output) {
        if (createVideoSink() && createLocalVideoSink()) {
            add(m_videosink);
            add(m_localvideosink);
            if (m_vsinkfilter) {
                add(m_vsinkfilter);
                link(m_vsinkfilter, m_videosink);
            }
            if (m_lvsinkfilter) {
                add(m_lvsinkfilter);
                link(m_lvsinkfilter, m_localvideosink);
            }
        }
    }

    if (input && output) {
        if (m_lvsinkfilter) {
            link(m_videoinputtee, m_lvsinkfilter);
        } else {
            link(m_videoinputtee, m_localvideosink);
        }
    }
    m_video_enabled = true;
    return true;
}

/** @brief Create audio source and sink. */
bool QPipeline::enableAudio()
{
    if (createAudioSource() && createAudioSink()) {
        add(m_source);
        add(m_sink);
        return true;
    } else {
        QPLOG() << "creating sink our source failed" << std::endl;
        m_valid = false;
        return false;
    }
    m_audio_enabled = true;
}

GstElement *QPipeline::getAudioSource()
{
    return m_source;
}

GstElement *QPipeline::getAudioSink()
{
    return m_sink;
}

GstElement *QPipeline::getVideoSource()
{
    return m_videosource;
}

GstElement * QPipeline::getVideoSink()
{
    return m_videosink;
}

GstElement * QPipeline::getLocalVideoSink()
{
    return m_localvideosink;
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

GstPad * QPipeline::getVideoSinkPad()
{
    GstPad *pad = NULL;
    if (m_vsinkfilter) {
        pad = gst_element_get_static_pad(m_vsinkfilter, "sink");
    } else if (m_videosink) {
        pad = gst_element_get_static_pad(m_videosink, "sink");
    }
    return pad;
}

GstPad * QPipeline::getVideoSourcePad()
{
    GstPad *pad = NULL;
    if (m_videoinputtee) {
        pad = gst_element_get_static_pad(m_videoinputtee, "src");
    } else if (m_vsourcefilter) {
        pad = gst_element_get_static_pad(m_vsourcefilter, "src");
    } else if (m_videosink) {
        pad = gst_element_get_static_pad(m_videosource, "src");
    }
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

std::string QPipeline::binToString(GstElement *bin)
{
    std::ostringstream o;
    bool done = false;
    GstElement *element = NULL;

    GstIterator *it = gst_bin_iterate_sorted(GST_BIN(bin));
    while(!done) {
        switch (gst_iterator_next(it, (void **) &element)) {
            case GST_ITERATOR_OK:
                o << gst_element_get_name(element) << ", ";
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
    return o.str();
}

std::string QPipeline::describe()
{
    std::ostringstream o;

    o << "Pipeline: " << gst_element_get_name(m_pipe) << std::endl 
      << " momentalni stav: " << current_state() 
      << " cekajici stav: " << pending_state() << std::endl;

    o << "Elements: ";
    o << binToString(m_pipe);
    return o.str();
}

bool QPipeline::createFilters()
{
    m_sourcefilter = gst_element_factory_make("capsfilter", "sourcefilter");
    g_assert(m_sourcefilter);
    GstCaps *fixed = gst_caps_new_simple("audio/x-raw-int",
        "rate", G_TYPE_INT, 8000,
        "channels", G_TYPE_INT, 1,
        "width", G_TYPE_INT, 16,
        NULL);
    g_assert(fixed);    
    g_object_set(G_OBJECT(m_sourcefilter), "caps", fixed, NULL);
    gst_caps_unref(fixed);
    add(m_sourcefilter);

    m_sinkfilter = NULL;
    return true;
}

bool QPipeline::isValid()
{
    return m_valid;
}

bool QPipeline::isVideoEnabled()
{
    return m_video_enabled;
}

bool QPipeline::isAudioEnabled()
{
    return m_audio_enabled;
}

