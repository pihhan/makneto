
#include <sstream>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include "qpipeline.h"
#include "testmediaconfig.h"

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
      m_valid(false), m_video_enabled(false), m_audio_enabled(false),
      m_audioInputEnabled(false), m_videoInputEnabled(false), 
      m_audioOutputEnabled(false), m_videoOutputEnabled(false)
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
    m_config = TestMediaConfig();

    m_valid = true;
}

QPipeline::~QPipeline()
{
    setState(GST_STATE_NULL);

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

bool QPipeline::remove(GstElement *element)
{
    return gst_bin_remove(GST_BIN(m_pipe), element);
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

/** \brief Helper class to simplify creating several input or output devices
    from configuration.
    \param d Device configuration.
    \param source Pointer to element pointer, that will be filled with created element.
    \param filterelem Pointer to element pointer, that will be filled with created filter, if configuration has one.
    \param name Name of element, that will be created.
*/
bool QPipeline::createElementsFromDevice(
    const MediaDevice &d,
    GstElement **source,
    GstElement **filterelem,
    const std::string &name)
{
    GError *err = NULL;

    std::string elname(name);
    if (elname.empty())
        elname = d.element();

    if (source) {
        const gchar *elemname = d.element().c_str();
        const gchar *uniquename = NULL;
        if (!name.empty())
            uniquename = name.c_str();
        *source = gst_element_factory_make(elemname, uniquename);
        if (! *source) {
            QPLOG() << "Could not make element \"" << d.element() << "\"" 
            << std::endl;
            return false;
        }

        if (!configureElement(*source, d.parameters)) {
            QPLOG() << "configureElement failed for " << elname << std::endl;
        }

        if (filterelem && !d.filter().empty()) {
            const char *filter = d.filter().c_str();
            std::string filtername = name + "-filter";
            if (d.filterType() == MediaDevice::BIN) {
                *filterelem = gst_parse_bin_from_description(filter, TRUE, &err);
                if (err) {
                    QPLOG() << " gst_parse_bin_from_description: " << 
                        elname << 
                        err->message << std::endl;
                    g_error_free(err);
                    return false;
                } else 
                    gst_object_set_name(GST_OBJECT(*filterelem), filtername.c_str());
            } else if (d.filterType() == MediaDevice::ELEMENT) {
                *filterelem = gst_element_factory_make(
                        filter, filtername.c_str() );
            }
            return (*source && *filterelem);
        } else
            return (*source != NULL);
    }
    return false;
}

/** @brief Create audio source element and its filter. 
    @return true if it was successful, false otherwise. */
bool QPipeline::createAudioSource()
{
    return createElementsFromDevice(
        m_config.audioInput(),
        &m_asource, &m_asourcefilter,
        "audio-source");
}

/** @brief Create audio output element.
    @return True if output was successfully created, false on the other case. */
bool QPipeline::createAudioSink()
{
    MediaDevice d = m_config.audioOutput();
    return createElementsFromDevice(d,
        &m_asink, &m_asinkfilter, "audio-sink");
}

bool QPipeline::createVideoSource()
{
    MediaDevice d = m_config.videoInput();

    return createElementsFromDevice(d,
        &m_videosource, &m_vsourcefilter, "video-source");
}

/** @brief Create output window for displaying of remote content, 
    and filter it will use.
*/
bool QPipeline::createVideoSink()
{
    MediaDevice d = m_config.videoOutput();
    return createElementsFromDevice(d,
        &m_videosink, &m_vsinkfilter, "remote-video-sink");
}

/** @brief Create element for local video input loopback, as preview.
*/
bool QPipeline::createLocalVideoSink()
{
    MediaDevice d = m_config.localVideoOutput();
    bool created = createElementsFromDevice(d,
        &m_localvideosink, &m_lvsinkfilter, "local-video-sink");
    if (m_localvideosink) {
        // tee element needs to turn off sync on second video output
        g_object_set(G_OBJECT(m_localvideosink), "sync", FALSE, NULL);
    }
    return created;
}

/** @brief Enable local video sink, create it if not created already. */
bool QPipeline::enableLocalVideoSink()
{
    if (!m_localvideosink) {
        if (!createLocalVideoSink()) {
            QPLOG() << "Creation of video local sink failed." << std::endl;
            return false;
        }
    } else {
        bool success = true;
        success = success && add(m_localvideosink);
        if (m_lvsinkfilter) {
            success = success && add(m_lvsinkfilter);
            success = success && link(m_lvsinkfilter, m_localvideosink);
            success = success && link(m_videoinputtee, m_lvsinkfilter);
        } else {
            success = success && link(m_videoinputtee, m_localvideosink);
        }
        return success;
    }
    return false;
}

/** @brief Disable local preview of video input. */
bool QPipeline::disableLocalVideoSink()
{
    GstPad *src = NULL;
    bool success = true;
    if (m_lvsinkfilter) 
        src = gst_element_get_pad(m_lvsinkfilter, "src");
    else 
        src = gst_element_get_pad(m_localvideosink, "src");

    if (src) {
        GstPad *dst = gst_pad_get_peer(src);
        if (dst) {
            success = success && gst_pad_unlink(src, dst);
        }
    }
    success = success && remove(m_lvsinkfilter);
    success = success && remove(m_localvideosink);
    return success;
}

/** @brief Create element that will split input to network and local preview. */
bool QPipeline::createVideoTee()
{
    m_videoinputtee = gst_element_factory_make("tee", "video-source-tee");
    return (m_videoinputtee != NULL);
}

/** @brief Enable video input elements, add them to pipeline, and link them.
    @return true if all was successful, false otherwise. */
bool QPipeline::enableVideoInput()
{
    if (m_video_enabled)
        return false;
    bool success = true;
    bool linked = true;
    bool added = true;
    bool use_tee = true;

    if (createVideoSource()) {
        QPLOG() << "video source created." << std::endl;
        added = added && add(m_videosource);
        if (use_tee) { 
            if (createVideoTee()) {
                added = added && add(m_videoinputtee);
            } else return false;
        }
        if (m_vsourcefilter) {
            added = added && add(m_vsourcefilter);
            linked = linked && link(m_videosource, m_vsourcefilter);
            if (use_tee) {
                linked = linked && link(m_vsourcefilter, m_videoinputtee);
            } else {
            }
        } else { 
            if (use_tee) 
                linked = linked && link(m_videosource, m_videoinputtee);
        }
    } else {
        QPLOG() << "Failed to create video source." << std::endl;
        return false;
    }
   
    success = added && linked;
    if (!success) {
        QPLOG() << "Failed to enable video source, added: " 
            << added << " linked: " << linked << std::endl;
    }
    m_videoInputEnabled = success;
    return success;
}

/** @brief Disable video input and delete input elements. */
bool QPipeline::disableVideoInput()
{
    bool success = true;
    if (m_videosource) {
        success = success && remove(m_videosource);
        m_videosource = NULL;
    }
    if (m_vsourcefilter) {
        success = success && remove(m_vsourcefilter);
        m_vsourcefilter = NULL;
    }
    if (m_videoinputtee) {
        success = success && remove(m_videoinputtee);
        m_videoinputtee = NULL;
    }
    m_videoInputEnabled = false;
    return success;
}

/** @brief Create elements for video input and output. */
bool QPipeline::enableVideo(bool input, bool output)
{
    if (m_video_enabled)
        return false;
    bool success = true;
    bool use_tee = true;

    if (input) {
        if (!enableVideoInput()) {
            return false;
        }
    }
    if (output) {
        if (createVideoSink()) {
            QPLOG() << "video sink created." << std::endl;
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
    QPLOG() << "Enabled video with config: " << std::endl 
            << m_config.describe() << std::endl;
    return success;
}

/** @brief Enable audio input modules and link filters needed. 
    @return True if creating and linking was successful, false otherwise. */
bool QPipeline::enableAudioInput()
{
    bool source =true;
    if ((source = createAudioSource())) {
        add(m_asource);
        if (m_asourcefilter) {
            add(m_asourcefilter);
            source = source && link(m_asource, m_asourcefilter);
        }
    } 
    if (!source) {
        QPLOG() << "creating audio source failed: " 
            << m_config.describe() << std::endl;
        m_valid = false;
        return false;
    }
    m_audioInputEnabled = source;
    return source;
}

bool QPipeline::disableAudioInput()
{
    bool removed = true;
    if (m_asource) {
        removed = removed && remove(m_asource);
        m_asource = NULL;
    }
    if (m_asourcefilter) {
        removed = removed && remove(m_asourcefilter);
        m_asourcefilter = NULL;
    }
    m_audioInputEnabled = false;
    return removed;
}


/** @brief Enable audio output modules and link filters to them. 
    @return True if created and linked succesfully, false otherwise. */
bool QPipeline::enableAudioOutput()
{
    bool sink = true;
    if ((sink = createAudioSink())) {
        add(m_asink);
        if (m_asinkfilter) {
            add(m_asinkfilter);
            sink = sink && link(m_asinkfilter, m_asink);
        }
    } 
    if (!sink) {
        QPLOG() << "creating sink or source failed" 
            << m_config.describe() << std::endl;
        m_valid = false;
        return false;
    }
    m_audioOutputEnabled = sink;
    return sink;
}

/** @brief Remove audio output elements from pipeline. */
bool QPipeline::disableAudioOutput()
{
    bool removed = true;
    if (m_asink) {
        removed = removed && remove(m_asink);
        m_asink = NULL;
    }
    if (m_asinkfilter) {
        removed = removed && remove(m_asinkfilter);
        m_asinkfilter = NULL;
    }
    m_audioOutputEnabled = false;
    return removed;
}

/** @brief Create audio source and sink, create filters if needed. */
bool QPipeline::enableAudio()
{
    bool sink = true;
    bool source =true;
    if (m_audio_enabled)
        return false;
    source = enableAudioInput();
    sink = enableAudioOutput();

    if (!source || !sink) {
        QPLOG() << "creating sink or source failed" 
            << m_config.describe() << std::endl;
        m_valid = false;
        return false;
    }
    m_audio_enabled = true;
    QPLOG() << "Enabled audio with config: " << std::endl 
            << m_config.describe() << std::endl;
    return (source && sink);
}

GstElement *QPipeline::getAudioSource()
{
    if (m_asource)
        gst_object_ref(GST_OBJECT(m_asource));
    return m_asource;
}

GstElement *QPipeline::getAudioSink()
{
    if (m_asink)
        gst_object_ref(GST_OBJECT(m_asink));
    return m_asink;
}

GstElement *QPipeline::getVideoSource()
{
    if (m_videosource)
        gst_object_ref(GST_OBJECT(m_videosource));
    return m_videosource;
}

GstElement * QPipeline::getVideoSink()
{
    if (m_videosink)
        gst_object_ref(GST_OBJECT(m_videosink));
    return m_videosink;
}

GstElement * QPipeline::getLocalVideoSink()
{
    if (m_localvideosink)
        gst_object_ref(GST_OBJECT(m_localvideosink));
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

/** @brief Get referenced source pad from video input filter or element. */
GstPad * QPipeline::getVideoSourcePad()
{
    GstPad *pad = NULL;
    GstElement *sourceelm = NULL;
    if (m_videoinputtee) {
        sourceelm = m_videoinputtee;
    } else if (m_vsourcefilter) {
        sourceelm = m_vsourcefilter;
    } else if (m_videosource) {
        sourceelm = m_videosource;
    }
    if (sourceelm) {
        pad = gst_element_get_static_pad(sourceelm, "src");
        if (!pad) 
            pad = gst_element_get_request_pad(sourceelm, "src%d");
    } else {
        QPLOG() << "Video source is not created." << std::endl;
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
                        << " is linked " << gst_pad_is_linked(pad) 
                        << " " << std::endl;
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

/** @brief Debug watcher for new elements inside pipeline. */
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

/** @brief Debug watcher for removed elements from pipeline. */
void QPipeline::elementRemoved(GstBin *bin, GstElement *element, gpointer)
{
    gchar *n1, *n2;
    n1 = gst_element_get_name(element);
    n2 = gst_element_get_name(GST_ELEMENT(bin));
    QPLOG() << "removed element: " << n1
        << " from " << n2 << std::endl;
    g_free(n1);
    g_free(n2);
}

/** @brief Create string description of bin elements. */
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
                QPLOG() << "Iterator error" << std::endl;
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

/** @brief Called from decodebin signal for audio files.
    Connect incoming pad to audiosink pad from pipeline. 
*/
void QPipeline::onNewDecodedAudioPad(GstElement *object,
    GstPad *src,
    gboolean arg1,
    gpointer pipe_data)
{
    QPipeline *pipeline = (QPipeline *) pipe_data;
    if (!pipeline)
        return;

    GstPad *sinkpad = pipeline->getAudioSinkPad();
    if (!pipeline->link(src, sinkpad)) {
        QPLOG() << "link of decoded pad failed!" << std::endl;
    } else 
        QPLOG() << "link of decoded pad succeeded." << std::endl;
}

/** @brief Create description string of current pipeline, with state, elements.
    Designed for debugging, not for saving or restoring. */
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

// TODO: delete, it is obsolete
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
    return isVideoInputEnabled() && isVideoOutputEnabled();
}

bool QPipeline::isAudioEnabled()
{
    return isAudioInputEnabled() && isAudioOutputEnabled();
}

MediaConfig QPipeline::mediaConfig()
{
    return m_config;
}

/** @brief Set Gstreamer elements names and other parameters for pipeline. */
void QPipeline::setMediaConfig(const MediaConfig &c)
{
    m_config = c;
}

/** @brief Configure parameters from map of payloadparameters.
    It will check type of each parameter
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
            if (i->first == "xwindow-id" 
                && gst_element_implements_interface(e, GST_TYPE_X_OVERLAY)) {
                unsigned long winid = p.ulongValue();
                QPLOG() << "Configuring video sink to window id: " 
                    << winid << std::endl;
                gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(e), winid);
                continue;
            } else {
                QPLOG() << "object spec not found for property " 
                    << i->first << " on object " << objname << std::endl;
                success = false;
                continue;
            }
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
            case G_TYPE_BOOLEAN:
                g_object_set(G_OBJECT(e),
                    paramname, (gboolean) p.intValue(), NULL);
                break;
            case G_TYPE_UINT:
                g_object_set(G_OBJECT(e),
                    paramname, p.uintValue(), NULL);
                break;
                
        }
        QPLOG() << "Setting parameter " << paramname 
                << " of " << objname << std::endl;

    }
    g_free(objname);
    return true;
}

GstElement * QPipeline::getElement(const char *name)
{
    return gst_bin_get_by_name(GST_BIN(m_pipe), name);
}

bool QPipeline::isAudioInputEnabled()
{
    return m_audioInputEnabled;
}

bool QPipeline::isAudioOutputEnabled()
{
    return m_audioOutputEnabled;
}

bool QPipeline::isVideoInputEnabled()
{
    return m_videoInputEnabled;
}

bool QPipeline::isVideoOutputEnabled()
{
    return m_videoOutputEnabled;
}

/** \brief Create string description of given pad.
    \param pad Pad to describe
    \param display_peer Whether to try display name and caps of connected pad
    Useful for debugging. 
*/
std::string QPipeline::describePad(GstPad *pad, bool display_peer)
{
    std::ostringstream o;
    gchar * name = gst_pad_get_name(pad);

    char direction = '?';
    switch (gst_pad_get_direction(pad)) {
        case GST_PAD_UNKNOWN:  direction = '?'; break;
        case GST_PAD_SINK:      direction = '<'; break;
        case GST_PAD_SRC:       direction = '>'; break;
    }
    o << "pad " << name << direction << " ";
    GstElement *parent = gst_pad_get_parent_element(pad);
    if (parent) {
        gchar * parentname = gst_element_get_name(parent);
        o << " of " << parentname << " ";
        g_free(parentname);
        gst_object_unref(parent);
    }

    bool linked = gst_pad_is_linked(pad);
    if (linked)
        o << " linked ";
    if (gst_pad_is_active(pad))
        o << " active ";
    if (gst_pad_is_blocking(pad)) 
        o << " blocking ";
    if (gst_pad_is_blocked(pad))
        o << " blocked ";

    GstCaps *caps = gst_pad_get_caps(pad);
    char * capsstr = gst_caps_to_string(caps);
    o << " caps=[" << capsstr << "] ";
    g_free(capsstr);
    gst_caps_unref(caps);

    GstCaps *caps_nego = gst_pad_get_negotiated_caps(pad);
    if (caps_nego) {
        char *caps_nego_str = gst_caps_to_string(caps_nego);
        o << " negotiated=[" << caps_nego_str << "] ";
        gst_caps_unref(caps_nego);
    }

    if (display_peer) {
        GstPad *peer = gst_pad_get_peer(pad);
        if (peer) {
            gchar * peername = gst_pad_get_name(peer);
            o << " peer=" << peername; 
            GstCaps *peercaps = gst_pad_get_caps(peer);
            if (peercaps) {
                gchar *peer_caps_str = gst_caps_to_string(peercaps);
                o << " with caps=[" << peer_caps_str << "] ";
                g_free(peer_caps_str);
                gst_caps_unref(peercaps);
            }
            g_free(peername);

            gst_object_unref(peer);
        } else {
            o << " (no peer)";
        }
    }


    g_free(name);
    return o.str();
}

