
#include "logger/logger.h"
#include "mediaconfig.h"
#include "avoutput.h"
#include "qpipeline.h"

#include "audiowatcher.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

using namespace farsight;

#define QPLOG() (LOGGER(logit))

/** @brief Simple class to allocate volume information for 
    specified count of channels.
*/
class VolumeInformation 
{
    public:

    void freeChannels()
    {
        if (levels) {
            delete levels;
            levels = 0;
        }
        if (decays) {
            delete decays;
            decays = 0;
        }
        if (peaks) {
            delete peaks;
            peaks = 0;
        }
    }

    void allocateChannels(int channels)
    {
        if (channels <= 0)
            return;
        freeChannels();

        levels = new double[channels];
        peaks = new double[channels];
        decays = new double[channels];
    }

    /** @brief Create volume information with specified number of channels.
        Volume values are undefined until you write something into them. */
    VolumeInformation(int channels)
        : channels(channels), levels(0), peaks(0), decays(0)
    {
        allocateChannels(channels);
    }

    virtual ~VolumeInformation()
    {
        freeChannels();
    }

    int channels;
    double *levels;
    double *peaks;
    double *decays;
};



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

        m_level = gst_element_factory_make("level", NULL);
        m_volume = gst_element_factory_make("volume", NULL);

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

bool AVOutput::disableAudio()
{
    bool success = true;
    success = success && m_pipeline->remove(m_asink);
    if (m_afilter)
        success = success && m_pipeline->remove(m_afilter);
    return success;
}

bool AVOutput::disableVideo()
{
    bool success = true;
    success = success && m_pipeline->remove(m_vsink);
    if (m_vfilter)
        success = success && m_pipeline->remove(m_vfilter);
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

/** @brief Parse values from gstreamer level message and 
    use it to send updateVolumes() call. 
    Pointers to doubles with volumes will be freed after
    updateVolumes call of audio watcher, if you want to keep
    them, you have to copy them. */
void AVOutput::parseLevelMessage(GstMessage *msg)
{
    GstStructure *s = msg->structure;
    if (s && gst_structure_has_name(s, "level")) {
        const GValue *peaks = gst_structure_get_value(s, "peak");
        const GValue *decays = gst_structure_get_value(s, "decay");
        const GValue *levels = gst_structure_get_value(s, "rms");

        int channels = gst_value_list_get_size(levels);

        VolumeInformation vols(channels);

        if (peaks && decays && levels) {
            for (unsigned int channel=0; 
                    channel < gst_value_list_get_size(peaks) ;
                    channel++) {
                const GValue *v = gst_value_list_get_value(peaks, channel);
                vols.peaks[channel] = g_value_get_double(v);
            }

            for (unsigned int channel=0; 
                    channel < gst_value_list_get_size(decays);
                    channel++) {
                const GValue *v = gst_value_list_get_value(decays, channel);
                vols.decays[channel] = g_value_get_double(v);
            }

            for (unsigned int channel=0; 
                    channel < gst_value_list_get_size(levels);
                    channel++) {
                const GValue *v = gst_value_list_get_value(levels, channel);
                vols.levels[channel] = g_value_get_double(v);
            }

            if (m_audiowatcher)
                m_audiowatcher->updateVolumes(channels, 
                    vols.levels, vols.peaks, vols.decays);
        }
    } else {
        g_warning("AVOutput::parseLevelMessage called not on level message, "
            "but %s", gst_structure_get_name(s));
    }
}

bool AVOutput::handleLevelMessage(GstMessage *msg)
{
    if (m_audiowatcher) {
        parseLevelMessage(msg);
        return true;
    } else
        return false;
}

void AVOutput::expose()
{
    if (m_vsink && GST_IS_X_OVERLAY(m_vsink)) 
        gst_x_overlay_expose(GST_X_OVERLAY(m_vsink));
    else
        g_error("Tried to expose on null video sink, or sink is not XOverlay");
}

/** @brief Set X11 window id to video output element. */
void AVOutput::setWindowId(unsigned long id)
{
    if (m_vsink && GST_IS_X_OVERLAY(m_vsink))
        gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(m_vsink), id);
    else
        g_error("Tried to set window id on null video sink, or sink is not XOverlay");
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

double AVOutput::volume() const
{
    if (m_volume) {
        double vol = -1.0;
        g_object_get(GST_OBJECT(m_volume), "volume", &vol, NULL);
        return vol;
    } else
        return -1.0;
}

void AVOutput::setVolume(double volume)
{
    if (m_volume) {
        g_object_set(GST_OBJECT(m_volume), "volume", volume, NULL);
    }
}

bool AVOutput::muted() const
{
    if (m_volume) {
        gboolean muted = false;
        g_object_get(GST_OBJECT(m_volume), "mute", &muted, NULL);
        return muted;
    } else
        return true;
}

void AVOutput::setMuted(bool muted)
{
    if (m_volume) {
        g_object_set(GST_OBJECT(m_volume),
            "mute", (gboolean) muted, 
            NULL);
    }
}

