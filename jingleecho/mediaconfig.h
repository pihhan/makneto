
#ifndef MEDIACONFIG_H
#define MEDIACONFIG_H

#include <string>
#include "payloadparameter.h"

/** @brief Class for description of one device in gstreamer. */
class MediaDevice {
    public:
    /** @brief Name of gstreamer element. */
    std::string element() const;
    /** @brief Description of gstreamer bin, to be attached before or after
    source or sink element itself. 
    It might be path to /dev tree, but format depends on module itself. */
    std::string filter() const;
    /** @brief Path to device, used as device parameter for element. */
    std::string path() const;

    void setElement(const std::string &element);
    void setFilter(const std::string &filter);

    void addParameter(const PayloadParameter &p);

    bool haveParameter(const std::string &key) const;
    PayloadParameter parameter(const std::string &key) const;

    PayloadParameterMap parameters;
    private:

    std::string m_element;
    std::string m_filter;
};

/** @brief Class to carry configuration of input and output devices.
*/
class MediaConfig {
    public:

    MediaDevice videoInput();
    MediaDevice videoOutput();
    MediaDevice audioInput();
    MediaDevice audioOutput();
    MediaDevice ringOutput();

    void setVideoInput(const MediaDevice &c);
    void setVideoOutput(const MediaDevice &c);
    void setAudioInput(const MediaDevice &c);
    void setAudioOutput(const MediaDevice &c);
    void setRingOutput(const MediaDevice &c);

    std::string videoInputElement();
    std::string videoInputDevice();
    std::string videoOutputElement();
    std::string videoOutputDevice();

    std::string audioInputElement();
    std::string audioInputDevice();
    std::string voiceOutputElement();
    std::string voiceOutputDevice();
    std::string ringOutputElement();
    std::string ringOutputDevice();

    void setVideoInputElement(const std::string &s);
    void setVideoInputDevice(const std::string &s);
    void setVideoOutputElement(const std::string &s);
    void setVideoOutputDevice(const std::string &s);

    void setAudioInputElement(const std::string &s);
    void setAudioInputDevice(const std::string &s);
    void setVoiceOutputElement(const std::string &s);
    void setVoiceOutputDevice(const std::string &s);
    void setRingOutputElement(const std::string &s);
    void setRingOutputDevice(const std::string &s);

    private:
    std::string m_videoInputElement;
    std::string m_videoInputDevice;
    std::string m_videoOutputElement;
    std::string m_videoOutputDevice;

    std::string m_audioInputElement;
    std::string m_audioInputDevice;
    std::string m_voiceOutputElement;
    std::string m_voiceOutputDevice;
    std::string m_ringOutputElement;
    std::string m_ringOutputDevice;

    MediaDevice m_videoInput;
    MediaDevice m_videoOutput;
    MediaDevice m_audioInput;
    MediaDevice m_audioOutput;
    MediaDevice m_ringOutput;
};

#endif
