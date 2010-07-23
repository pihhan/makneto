
#include <sstream>
#include <glib.h>
#include "mediaconfig.h"

MediaConfig::MediaConfig()
{
    m_videoInput = deviceFromEnvironment("VIDEOSOURCE", DEFAULT_VIDEOSOURCE,
        DEFAULT_VIDEOSOURCE_FILTER);
    m_videoOutput = deviceFromEnvironment("VIDEOSINK", DEFAULT_VIDEOSINK,
        DEFAULT_VIDEOSINK_FILTER);
    m_audioInput = deviceFromEnvironment("AUDIOSOURCE", DEFAULT_AUDIOSOURCE,
        DEFAULT_AUDIOSOURCE_FILTER);
    m_audioOutput = deviceFromEnvironment("AUDIOSINK", DEFAULT_AUDIOSINK,
        DEFAULT_AUDIOSINK_FILTER);
    m_ringOutput = deviceFromEnvironment("RINGSINK", DEFAULT_AUDIOSINK,
        DEFAULT_AUDIOSINK_FILTER);
}

MediaDevice MediaConfig::deviceFromEnvironment(
    const std::string &environ,
    const std::string &default_element,
    const std::string &default_filter)
{
    std::string envf = environ + "_FILTER";
    MediaDevice d;
    const char *env_element = g_getenv(environ.c_str());
    const char *env_filter = g_getenv(envf.c_str());

    if (env_element) {
        d.setElement(env_element);
    } else {
        d.setElement(default_element);
    }
    if (env_filter) {
        d.setElement(env_filter);
    } else {
        d.setElement(default_filter);
    }
    return d;
}

MediaDevice MediaConfig::videoInput() const
{
    return m_videoInput;
}

MediaDevice MediaConfig::videoOutput() const
{
    return m_videoOutput;
}

MediaDevice MediaConfig::localVideoOutput() const
{
    return m_localVideoOutput;
}

MediaDevice MediaConfig::audioInput() const
{
    return m_audioInput;
}

MediaDevice MediaConfig::audioOutput() const
{
    return m_audioOutput;
}

MediaDevice MediaConfig::ringOutput() const
{
    return m_ringOutput;
}


void MediaConfig::setVideoInput(const MediaDevice &c)
{
    m_videoInput = c;
}

void MediaConfig::setVideoOutput(const MediaDevice &c)
{
    m_videoOutput = c;
}

void MediaConfig::setLocalVideoOutput(const MediaDevice &c)
{
    m_localVideoOutput = c;
}

void MediaConfig::setAudioInput(const MediaDevice &c)
{
    m_audioInput = c;
}

void MediaConfig::setAudioOutput(const MediaDevice &c)
{
    m_audioOutput = c;
}

void MediaConfig::setRingOutput(const MediaDevice &c)
{
    m_ringOutput = c;
}

/** @brief Create description of all devices in this config. */
std::string MediaConfig::describe() const
{
    std::ostringstream o;
    o << "VideoInput: " << m_videoInput.describe() << std::endl;
    o << "VideoOutput: " << m_videoOutput.describe() << std::endl;
    o << "AudioInput: " << m_audioInput.describe() << std::endl;
    o << "AudioOutput: " << m_audioOutput.describe() << std::endl;
    o << "RingOutput: " << m_ringOutput.describe() << std::endl;
    o << "LocalVideoOutput: " << m_localVideoOutput.describe() << std::endl;
    return o.str();
}


/*
 *
 * MediaDevice
 *
 */
MediaDevice::MediaDevice()
{
}

MediaDevice::MediaDevice(const std::string &element)
    : m_element(element)
{
}


std::string MediaDevice::element() const
{
    return m_element;
}

/** @brief Description of gstreamer bin, to be attached before or after
source or sink element itself. 
It might be path to /dev tree, but format depends on module itself. */
std::string MediaDevice::filter() const
{
    return m_filter;
}

/** @brief Path to device, used as device parameter for element. */
std::string MediaDevice::path() const 
{
    PayloadParameterMap::const_iterator i= parameters.find("device");
    if (i!=parameters.end()) {
        return i->second.stringValue();
    } else {
        return std::string();
    }
}

void MediaDevice::setElement(const std::string &element)
{
    m_element = element;
}

void MediaDevice::setFilter(const std::string &filter)
{
    m_filter = filter;
}

void MediaDevice::addParameter(const PayloadParameter &p)
{
    parameters.insert(make_pair(p.name(),p));
}

/** @brief Check if parameter is present. */
bool MediaDevice::haveParameter(const std::string &key) const
{
    PayloadParameterMap::const_iterator i= parameters.find(key);
    return (i!=parameters.end());
}

PayloadParameter MediaDevice::parameter(const std::string &key) const
{
    PayloadParameterMap::const_iterator i= parameters.find(key);
    if (i != parameters.end()) {
        return i->second;
    } else {
        return PayloadParameter("", 0);
    }
}

/** @brief Create string with description of this device. */
std::string MediaDevice::describe() const
{
    std::ostringstream o;
    o << "Device " << m_element;
    if (m_filter.empty()) {
        o << " without filter ";
    } else {
        o << " with filter=\"" << m_filter << "\"";
    }
    for (PayloadParameterMap::const_iterator i=parameters.begin();
        i!= parameters.end(); i++) 
    {
        o << " " << i->first << "=\"" << i->second.stringValue() << "\"";
    }
    return o.str();
}

