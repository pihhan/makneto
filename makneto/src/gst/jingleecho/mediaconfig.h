
#ifndef MEDIACONFIG_H
#define MEDIACONFIG_H

#include <string>
#include "payloadparameter.h"

#define DEFAULT_AUDIOSOURCE "audiotestsrc"
#define DEFAULT_AUDIOSINK   "pulsesink"

#define DEFAULT_AUDIOSOURCE_FILTER "audioconvert ! audioresample"
#define DEFAULT_AUDIOSINK_FILTER    "audioconvert ! audioresample"

#define DEFAULT_VIDEOSOURCE "v4l2src"
#define DEFAULT_VIDEOSOURCE_FILTER  "capsfilter caps=video/x-raw-yuv,width=640 ! ffmpegcolorspace ! videoscale"

#define DEFAULT_VIDEOSINK   "xvimagesink"
#define DEFAULT_VIDEOSINK_FILTER "ffmpegcolorspace ! videoscale"

/** @brief Class for description of one device in gstreamer. */
class MediaDevice 
{
    public:

    MediaDevice();
    MediaDevice(const std::string &element);

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

    std::string describe() const;

    PayloadParameterMap parameters;
    private:

    std::string m_element;
    std::string m_filter;
};

/** @brief Class to carry configuration of input and output devices.
    This class should carry all configuration needed.
    In future, it might also support reading and writing config file,
    or child reimplementation.
*/
class MediaConfig 
{
    public:

    MediaConfig();
    MediaDevice videoInput() const;
    MediaDevice videoOutput() const;
    MediaDevice localVideoOutput() const;
    MediaDevice audioInput() const;
    MediaDevice audioOutput() const;
    MediaDevice ringOutput() const;

    void setVideoInput(const MediaDevice &c);
    void setVideoOutput(const MediaDevice &c);
    void setLocalVideoOutput(const MediaDevice &c);
    void setAudioInput(const MediaDevice &c);
    void setAudioOutput(const MediaDevice &c);
    void setRingOutput(const MediaDevice &c);

    static MediaDevice deviceFromEnvironment(
    const std::string &environ,
    const std::string &default_element,
    const std::string &default_filter);

    std::string describe() const;

    private:
    MediaDevice m_videoInput;
    MediaDevice m_videoOutput;
    MediaDevice m_localVideoOutput;
    MediaDevice m_audioInput;
    MediaDevice m_audioOutput;
    MediaDevice m_ringOutput;
};

#endif
