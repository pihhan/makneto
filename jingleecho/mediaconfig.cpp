
#include "mediaconfig.h"

#if 0
std::string MediaConfig::videoInputElement()
{
    return m_videoInputElement
}
std::string MediaConfig::videoInputDevice()
{
    return m_videoInputDevice;
}

std::string MediaConfig::videoOutputElement()
{
    return m_videoOutputElement;
}

std::string MediaConfig::videoOutputDevice()
{
    return m_videoOutputDevice;
}


std::string MediaConfig::audioInputElement()
{
    return m_audioInputElement;
}

std::string MediaConfig::audioInputDevice()
{
    return m_audioInputDevice;
}

std::string MediaConfig::voiceOutputElement()
{
    return m_voiceOutputElement;
}

std::string MediaConfig::voiceOutputDevice()
{
    return m_voiceOutputDevice;
}

std::string MediaConfig::ringOutputElement()
{
    return m_ringOutputElement;
}

std::string MediaConfig::ringOutputDevice()
{
    return m_ringOutputDevice;
}


void MediaConfig::setVideoInputElement(const std::string &s)
{
    m_videoInputElement = s;
}

void MediaConfig::setVideoInputDevice(const std::string &s)
{
    m_videoInputDevice = s;
}

void MediaConfig::setVideoOutputElement(const std::string &s)
{
    m_videoOutputElement = s;
}
void MediaConfig::setVideoOutputDevice(const std::string &s)
{
    m_videoOutputDevice = s;
}

void MediaConfig::setAudioInputElement(const std::string &s)
{
    m_audioInputElement = s;
}
void MediaConfig::setAudioInputDevice(const std::string &s)
{
    m_audioInputDevice = s;
}
void MediaConfig::setVoiceOutputElement(const std::string &s)
{
    m_voiceOutputElement = s;
}
void MediaConfig::setVoiceOutputDevice(const std::string &s)
{
    m_voiceOutputDevice = s;
}
void MediaConfig::setRingOutputElement(const std::string &s)
{
    m_ringOutputElement = s;
}
void MediaConfig::setRingOutputDevice(const std::string &s)
{
    m_ringOutputDevice = s;
}
#endif

MediaDevice MediaConfig::videoInput()
{
    return m_videoInput;
}

MediaDevice MediaConfig::videoOutput()
{
    return m_videoOutput;
}

MediaDevice MediaConfig::audioInput()
{
    return m_audioInput;
}

MediaDevice MediaConfig::audioOutput()
{
    return m_audioOutput;
}

MediaDevice MediaConfig::ringOutput()
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



/*
 *
 * MediaDevice
 *
 */
std::string MediaDevice::element()
{
    return m_element;
}

/** @brief Description of gstreamer bin, to be attached before or after
source or sink element itself. 
It might be path to /dev tree, but format depends on module itself. */
std::string MediaDevice::filter()
{
    return m_filter;
}

/** @brief Path to device, used as device parameter for element. */
std::string MediaDevice::path()
{
    PayloadParametersMap::iterator i= parameters.find("device");
    if (i!=parameters.end()) {
        return *i;
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
bool MediaDevice::haveParameter(const std::string &key)
{
    PayloadParameterMap::iterator i= parameters.find(key);
    return (i!=parameters.end());
}

PayloadParameter MediaDevice::parameter(const std::string &key)
{
    PayloadParameterMap::iterator i= parameters.find(key);
    if (i != parameters.end()) {
        return *i;
    } else {
        return PayloadParameter("", 0);
    }
}

