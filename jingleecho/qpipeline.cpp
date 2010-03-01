
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
    : m_asource(0), m_asourcefilter(0), m_asink(0), m_asinkfilter(0), 
      m_videosource(0), m_vsourcefilter(0), m_videosink(0), m_vsinkfilter(0),
      m_localvideosink(0), m_lvsinkfilter(0), m_videoinputtee(0),
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
    GstIterator *i = gst_bin_iterate_elements(GST_BIN(m_pipe));
    // walk pipeline and remove all elements
    while (i) {
        GstElement *e = NULL;
        switch (gst_iterator_next(i, (void **) &e)) {
            case GST_ITERATOR_OK:
                if (!gst_bin_remove(GST_BIN(m_pipe), e)) {
                    g_error("qpipeline remove failed on destroy for element %s",
                        gst_object_get_name(GST_OBJECT(e)));
                }
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(i);
                break;
            case GST_ITERATOR_DONE:
                gst_iterator_free(i);
                i = NULL;
                break;
            case GST_ITERATOR_ERROR:
                g_error("qpipeline iterator error on destroy");
                gst_iterator_free(i);
                i = NULL;
                break;
        }
    }

    // unref all local elements that are allocated
    if (m_videosource) 
        gst_object_unref(m_videosource);
    if (m_vsourcefilter)
        gst_object_unref(m_vsourcefilter);
    if (m_videosink)
        gst_object_unref(m_videosink);
    if (m_vsinkfilter)
        gst_object_unref(m_vsinkfilter);
    if (m_localvideosink)
        gst_object_unref(m_localvideosink);
    if (m_lvsinkfilter)
        gst_object_unref(m_lvsinkfilter);
    if (m_videoinputtee)
        gst_object_unref(m_videoinputtee);

    // unref pipeline itself
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

/** @brief Create audio source element and its filter. 
    @return true if it was successful, false otherwise. */
bool QPipeline::createAudioSource()
{
    GError *err = NULL;
    MediaDevice d = m_config.audioInput();

    m_asource = gst_element_factory_make(d.element().c_str(), "audio-source");
    if (!m_asource) {
        QPLOG() << "Cannot make element \"" << d.element() << "\"" << std::endl;
        return false;
    }
    if (d.element() == "audiotestsrc")
        g_object_set(G_OBJECT(m_asource), "is-live", (gboolean) TRUE, NULL);

    if (!d.filter().empty()) {
        const char *filter = d.filter().c_str();
        m_asourcefilter = gst_parse_bin_from_description(filter, TRUE, &err);
        if (err) {
            QPLOG() << " gst_parse_bin_from_description: " <<
                err->message << std::endl;
                return false;
        } else {
            gst_object_set_name(GST_OBJECT(m_asourcefilter), "audio-source-filter");
        }
    }
    return (m_asource != NULL);
}

/** @brief Create audio output element.
    @return True if output was successfully created, false on the other case. */
bool QPipeline::createAudioSink()
{
    GError *err = NULL;
    MediaDevice d = m_config.audioInput();

    m_asink = gst_element_factory_make(d.element().c_str(), "audio-sink");
    if (!m_asink) {
        QPLOG() << "Cannot make element \"" << d.element() << "\"" << std::endl;
        return false;
    }

    if (!d.filter().empty()) {
        const char *filter = d.filter().c_str();
        m_asinkfilter = gst_parse_bin_from_description(filter, TRUE, &err);
        if (err) {
            QPLOG() << " gst_parse_bin_from_description: " <<
                err->message << std::endl;
                return false;
        } else {
            gst_object_set_name(GST_OBJECT(m_asinkfilter), "audio-sink-filter");
        }
    }
    return (m_asink != NULL);
}

bool QPipeline::createVideoSource()
{
    GError *err = NULL;
    MediaDevice d = m_config.videoInput();

    m_videosource = gst_element_factory_make(
        d.element().c_str(), "video-source");
    if (!m_videosource) {
        QPLOG() << "Video source element \"" << d.element()
            << "\" creation failed." << std::endl;
        return false;
    }
    if (d.element() == "videotestsrc") {
        g_object_set(G_OBJECT(m_videosource),
            "is-live", (gboolean) TRUE, 
            NULL);
    }

    if (!d.filter().empty()) {
        const char *filter = d.filter().c_str();
        m_vsourcefilter = gst_parse_bin_from_description(filter, TRUE, &err);
        if (err) {
            QPLOG() << "Parsing video filter bin failed: " << err->message << std::endl;
            return false;
        } else {
            gst_object_set_name(GST_OBJECT(m_vsourcefilter), "vsource-filter-bin");
        }
    }

    return (m_videosource != NULL);
}

/** @brief Create output window for displaying of remote content, 
    and filter it will use.
*/
bool QPipeline::createVideoSink()
{
    GError *err = NULL;
    MediaDevice d = m_config.videoOutput();
    m_videosink = gst_element_factory_make(
        d.element().c_str(), "remote-video-sink");
    if (!m_videosink) {
        QPLOG() << "Creating of " << d.element() << " failed." << std::endl;
        return false;
    }
    if (!d.filter().empty()) {
        m_vsinkfilter = gst_parse_bin_from_description(
            d.filter().c_str(), TRUE, &err);
        if (err) {
            QPLOG() << "Creating of video sink filter failed: " << err->message << std::endl;
        } else {
            gst_object_set_name(GST_OBJECT(m_vsinkfilter), "vsink-filter-bin");
        }
    }
    return (m_videosink != NULL);
}

/** @brief Create element for local video input loopback, as preview.
*/
bool QPipeline::createLocalVideoSink()
{
    GError *err = NULL;
    MediaDevice d = m_config.localVideoOutput();

    m_localvideosink = gst_element_factory_make(
        d.element().c_str(), "local-video-sink");
    if (!m_localvideosink) {
        QPLOG() << "Creating of local video sink element " 
            << d.element() << " failed." << std::endl;
        return false;
    }

    if (!d.filter().empty()) {
        m_lvsinkfilter = gst_parse_bin_from_description(
            d.filter().c_str(), TRUE, &err);
        if (err) {
            QPLOG() << "Creating of local video sink filter failed: " << err->message << std::endl;
        } else {
            gst_object_set_name(GST_OBJECT(m_lvsinkfilter), "vsink-filter-bin");
        }
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
    if (m_video_enabled)
        return false;
    bool success = true;
    bool use_tee = false;

    if (input) {
        if (createVideoSource()) {
            success = success && add(m_videosource);
            if (use_tee) { 
                if (createVideoTee()) {
                    success = success && add(m_videoinputtee);
                } else return false;
            }
            if (m_vsourcefilter) {
                success = success && add(m_vsourcefilter);
                success = success && link(m_videosource, m_vsourcefilter);
                if (use_tee)
                    success = success && link(m_vsourcefilter, m_videoinputtee);
            } else if (use_tee) {
                success = success && link(m_videosource, m_videoinputtee);
            }
        } else return false;
    }
    
    if (output) {
        if (createVideoSink()) {
            success = success && add(m_videosink);
            if (m_vsinkfilter) {
                success = success && add(m_vsinkfilter);
                success = success && link(m_vsinkfilter, m_videosink);
            }
        } else return false;
        if (use_tee) { 
            if (createLocalVideoSink()) {
                success = success && add(m_localvideosink);
                if (m_lvsinkfilter) {
                    success = success && add(m_lvsinkfilter);
                    success = success && link(m_lvsinkfilter, m_localvideosink);
                }
            } else {
                return false;
            }
        }
    }

    if (input && output && m_videoinputtee) {
        if (m_lvsinkfilter) {
            success = success && link(m_videoinputtee, m_lvsinkfilter);
        } else {
            success = success && link(m_videoinputtee, m_localvideosink);
        }
    }
    m_video_enabled = true;
    return success;
}

/** @brief Create audio source and sink, create filters if needed. */
bool QPipeline::enableAudio()
{
    bool sink = true;
    bool source =true;
    if (m_audio_enabled)
        return false;
    if ((source = createAudioSource())) {
        add(m_asource);
        if (m_asourcefilter) {
            add(m_asourcefilter);
            source = source && link(m_asource, m_asourcefilter);
        }
    } 
    if ((sink = createAudioSink())) {
        add(m_asink);
        if (m_asinkfilter) {
            add(m_asinkfilter);
            sink = sink && link(m_asinkfilter, m_asink);
        }
    } 
    if (!source || !sink) {
        QPLOG() << "creating sink or source failed" << std::endl;
        m_valid = false;
        return false;
    }
    m_audio_enabled = true;
    return (source && sink);
}

GstElement *QPipeline::getAudioSource()
{
    return m_asource;
}

GstElement *QPipeline::getAudioSink()
{
    return m_asink;
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
    GstElement *srce = m_asource;
    if (m_asourcefilter)
        srce = m_asourcefilter;
    GstPad *pad = gst_element_get_static_pad(srce, "src");
    return pad;
}

GstPad * QPipeline::getAudioSinkPad()
{
    GstElement *sinke = m_asink;
    if (m_asinkfilter)
        sinke = m_asinkfilter;
    GstPad *pad = gst_element_get_static_pad(sinke, "sink");
    return pad;
}

GstPad * QPipeline::getVideoSinkPad()
{
    GstPad *pad = NULL;
    GstElement *sink = NULL;
    if (m_vsinkfilter) {
        sink = m_vsinkfilter;
    } else if (m_videosink) {
        sink = m_videosink;
    }
    if (sink) {
        pad = gst_element_get_static_pad(sink, "sink");
        if (!pad)
            pad = gst_element_get_request_pad(sink, "sink%d");
    }
    return pad;
}

GstPad * QPipeline::getVideoSourcePad()
{
    GstPad *pad = NULL;
    GstElement *sourceelm = NULL;
    if (m_videoinputtee) {
        sourceelm = m_videoinputtee;
    } else if (m_vsourcefilter) {
        sourceelm = m_vsourcefilter;
    } else if (m_videosink) {
        sourceelm = m_videosink;
    }
    if (sourceelm) {
        pad = gst_element_get_static_pad(sourceelm, "src");
        if (!pad) 
            pad = gst_element_get_request_pad(sourceelm, "src%d");
    }
    if (!pad) {
        QPLOG() << "Video source pad not yet found" << std::endl;
        GstIterator *i = gst_element_iterate_src_pads(sourceelm);
        GstPad *pad = NULL;
        bool done = false;
        do {
            switch (gst_iterator_next(i, (void **) &pad)) {
                case GST_ITERATOR_DONE:
                    done = true;
                    break;
                case GST_ITERATOR_OK:
                    QPLOG() << "src pad: " << gst_pad_get_name(pad) 
                        << " is linked " << gst_pad_is_linked(pad) << std::endl;
                        gst_object_unref(pad);
                    break;
                case GST_ITERATOR_RESYNC:
                    gst_iterator_resync(i);
                    break;
                case GST_ITERATOR_ERROR:
                    done = true;
                    QPLOG() << "Src pad iterator error" << std::endl;
                    break;
            }
        } while (!done);
        gst_iterator_free(i);
    }
    return pad;
}


void QPipeline::elementAdded(GstBin *bin, GstElement *element, gpointer pipeline)
{
    gchar *n1, *n2;
    n1 = gst_element_get_name(element);
    n2 = gst_element_get_name(GST_ELEMENT(bin));
    QPLOG() << "added element: " << n1
        << " to " << n2 << std::endl;
    g_free(n1);
    g_free(n2);
}

void QPipeline::elementRemoved(GstBin *bin, GstElement *element, gpointer pipeline)
{
    gchar *n1, *n2;
    n1 = gst_element_get_name(element);
    n2 = gst_element_get_name(GST_ELEMENT(bin));
    QPLOG() << "removed element: " << n1
        << " from " << n2 << std::endl;
    g_free(n1);
    g_free(n2);
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
                o << gst_element_get_name(element);
                if (GST_STATE_PENDING(element) != GST_STATE_VOID_PENDING)
                    o << "(" << GST_STATE(element) 
                      << "/" << GST_STATE_PENDING(element) << ")";
                o << ", ";
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
      << " states current: " << current_state() 
      << " pending: " << pending_state() << std::endl;

    o << "Elements: ";
    o << binToString(m_pipe);
    return o.str();
}

bool QPipeline::createFilters()
{
    m_asourcefilter = gst_element_factory_make("capsfilter", "sourcefilter");
    if (!m_asourcefilter)
        return false;
    GstCaps *fixed = gst_caps_new_simple("audio/x-raw-int",
        "rate", G_TYPE_INT, 8000,
        "channels", G_TYPE_INT, 1,
        "width", G_TYPE_INT, 16,
        NULL);
    g_assert(fixed);    
    g_object_set(G_OBJECT(m_asourcefilter), "caps", fixed, NULL);
    gst_caps_unref(fixed);
    add(m_asourcefilter);

    m_asinkfilter = NULL;
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

MediaConfig QPipeline::mediaConfig()
{
    return m_config;
}

void QPipeline::setMediaConfig(const MediaConfig &c)
{
    m_config = c;
}

/** @brief configure parameters from map of payloadparameters.
    It will check type of parameter
*/
bool QPipeline::configureElement(GstElement *e, PayloadParameterMap p)
{
    bool success = true;
    char *objname = (gst_object_get_name(GST_OBJECT(e)));
    GObjectClass *klass = G_OBJECT_GET_CLASS(G_OBJECT(e));
    if (!klass) {
        QPLOG() << "object class not returned on object" << objname << std::endl;
        g_free(objname);
        return false;
    }

    for (PayloadParameterMap::iterator i=p.begin(); i!=p.end(); i++) {
        const char *paramname = i->first.c_str();
        PayloadParameter & p = i->second;
        GParamSpec *spec = NULL;
        spec = g_object_class_find_property(klass, paramname);
        if (!spec) {
            QPLOG() << "object spec not found for property " 
                << i->first << " on object " << objname << std::endl;
            success = false;
            continue;
        }
        switch (spec->value_type) {
            case G_TYPE_STRING:
                g_object_set(G_OBJECT(e), 
                    paramname, p.stringValue().c_str(), NULL);
                break;
            case G_TYPE_INT:
                g_object_set(G_OBJECT(e),
                    paramname, p.intValue(), NULL);
                break;
            case G_TYPE_UINT:
                g_object_set(G_OBJECT(e),
                    paramname, p.uintValue(), NULL);
                break;
                
        }

#if 0
        switch(i->second.type()) {
            case PayloadParameter::STRING:
                success = success && 
                    g_object_set(G_OBJECT(e), i->first.c_str(), 
                        i->second.stringValue().c_str(), NULL);
                break;
            case PayloadParameter::INT:
                success = success && 
                    g_object_set(G_OBJECT(e), i->first.c_str(), 
                        i->second.intValue(), NULL);
                break;
            case PayloadParameter::UINT:
                success = success && 
                    g_object_set(G_OBJECT(e), i->first.c_str(), 
                        i->second.uintValue(), NULL);
                break;
                
        }
#endif
    }
    g_free(objname);
    return true;
}

