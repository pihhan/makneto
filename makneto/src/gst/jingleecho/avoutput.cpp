
#include "logger/logger.h"
#include "mediaconfig.h"
#include "avoutput.h"
#include "qpipeline.h"

#include "audiowatcher.h"
#include <gst/interfaces/xoverlay.h>

#define QPLOG() (LOGGER(logit))

AVOutput::AVOutput(QPipeline *p)
    : m_vsink(0), m_asink(0), m_afilter(0), m_vfilter(0), m_level(0), 
      m_volume(0), m_pipeline(p), m_audiowatcher(0), m_videowatcher(0)
{
}

AVOutput::AVOutput(QPipeline *p, MediaDevice audio, MediaDevice video)
    : m_vsink(0), m_asink(0), m_afilter(0), m_vfilter(0), m_level(0), 
      m_volume(0),  m_pipeline(p), m_audiowatcher(0), m_videowatcher(0)
{
    m_audioconfig = audio;
    m_videoconfig = video;
}

AVOutput::~AVOutput()
{
    m_pipeline->remove(m_vsink);
    m_pipeline->remove(m_vfilter);
    m_pipeline->remove(m_asink);
    m_pipeline->remove(m_afilter);
}

bool AVOutput::enableAudio()
{
    bool success = true;
    if (createAudioSink()) {
        success = success && m_pipeline->add(m_asink);
        if (m_afilter) {
            success = success && m_pipeline->add(m_afilter);
            success = success && m_pipeline->link(m_afilter, m_asink);
        }
    } else {
        return false;
    }
    return success;
}

bool AVOutput::enableVideo()
{
    bool success = true;
    if (createVideoSink()) {
        success = success && m_pipeline->add(m_vsink);
        if (m_afilter) {
            success = success && m_pipeline->add(m_vfilter);
            success = success && m_pipeline->link(m_vfilter, m_vsink);
        }
    } else {
        return false;
    }
    return success;
}

bool AVOutput::audioEnabled()
{
    return (m_asink != NULL);
}
bool AVOutput::videoEnabled()
{
    return (m_vsink != NULL);
}

MediaDevice AVOutput::videoConfig() const
{
    return m_videoconfig;
}
MediaDevice AVOutput::audioConfig() const
{
    return m_audioconfig;
}

void AVOutput::setVideoConfig(const MediaDevice &d)
{
    m_videoconfig = d;
}
void AVOutput::setAudioConfig(const MediaDevice &d)
{
    m_audioconfig = d;
}

GstPad * AVOutput::getVideoSinkPad()
{
    GstPad *pad = NULL;
    if (m_vfilter) {
        pad = gst_element_get_pad(m_vfilter, "sink");
    } else {
        pad = gst_element_get_pad(m_vsink, "sink");
    }
    return pad;
}

GstPad * AVOutput::getAudioSinkPad()
{
    GstPad *pad = NULL;
    if (m_afilter) {
        pad = gst_element_get_pad(m_afilter, "sink");
    } else {
        pad = gst_element_get_pad(m_asink, "sink");
    }
    return pad;
}

GstElement *AVOutput::videoElement()
{
    return m_vsink;
}
GstElement *AVOutput::audioElement()
{
    return m_asink;
}
GstElement *AVOutput::videoFilterElement()
{
    return m_vfilter;
}
GstElement *AVOutput::audioFilterElement()
{
    return m_afilter;
}

std::string AVOutput::name()
{
    return participant;
}

void AVOutput::setName(const std::string &name)
{
    participant = name;
}

/** @brief Create output window for displaying of remote content, 
    and filter it will use.
*/
bool AVOutput::createVideoSink()
{
    GError *err = NULL;
    MediaDevice d = m_videoconfig;
    m_vsink = gst_element_factory_make(
        d.element().c_str(), NULL);
    if (!m_vsink) {
        QPLOG() << "Creating of " << d.element() << " failed." << std::endl;
        return false;
    }
    if (!QPipeline::configureElement(m_vsink, d.parameters)) {
        QPLOG() << " configureElement failed for video sink" << std::endl;
    }

    if (!d.filter().empty()) {
        m_vfilter = gst_parse_bin_from_description(
            d.filter().c_str(), TRUE, &err);
        if (err) {
            QPLOG() << "Creating of video sink filter failed: " << err->message << std::endl;
            return false;
        } else {
//            gst_object_set_name(GST_OBJECT(m_vfilter), "vsink-filter-bin");
        }
    }
    return (m_vsink != NULL);
}

/** @brief Create audio output element.
    @return True if output was successfully created, false on the other case. */
bool AVOutput::createAudioSink()
{
    GError *err = NULL;
    MediaDevice d = m_audioconfig;

    m_asink = gst_element_factory_make(d.element().c_str(), NULL);
    if (!m_asink) {
        QPLOG() << "Cannot make element \"" << d.element() << "\"" << std::endl;
        return false;
    }
    if (!QPipeline::configureElement(m_asink, d.parameters)) {
        QPLOG() << " configureElement failed for audio sink" << std::endl;
    }

    if (!d.filter().empty()) {
        const char *filter = d.filter().c_str();
        m_afilter = gst_parse_bin_from_description(filter, TRUE, &err);
        if (err) {
            QPLOG() << " gst_parse_bin_from_description: " <<
                err->message << std::endl;
                return false;
        } else {
            //gst_object_set_name(GST_OBJECT(m_asinkfilter), "audio-sink-filter");
        }
    }
    return (m_asink != NULL);
}


GstElement *AVOutput::levelElement()
{
    if (m_level)
        gst_object_ref(GST_OBJECT(m_level));
    return m_level;
}

GstElement *AVOutput::volumeElement()
{
    if (m_volume)
        gst_object_ref(GST_OBJECT(m_volume));
    return m_volume;
}

void AVOutput::registerAudioWatcher(GstAudioWatcher *watcher)
{
    m_audiowatcher = watcher;
}

void AVOutput::registerVideoWatcher(GstVideoWatcher *watcher)
{
    m_videowatcher = watcher;
}

bool AVOutput::handleLevelMessage(GstMessage *msg)
{
    if (m_audiowatcher) {
        m_audiowatcher->updateMessage(msg);
        return true;
    } else
        return false;
}

void AVOutput::expose()
{
    if (m_vsink) 
        gst_x_overlay_expose(GST_X_OVERLAY(m_vsink));
}

/** @brief Set X11 window id to video output element. */
void AVOutput::setWindowId(unsigned long id)
{
    if (m_vsink)
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(m_vsink), id);
}

Stream *AVOutput::audioStream() const
{
    return m_audioStream;
}

Stream *AVOutput::videoStream() const
{
    return m_videoStream;
}


void AVOutput::setAudioStream(Stream *s)
{
    m_audioStream = s;
}

void AVOutput::setVideoStream(Stream *s)
{
    m_videoStream = s;
}

