
#include "testmediaconfig.h"

/** @brief Create configuration for testing. 
    That means automatic generator of video and audio signal,
    and output to user display. */
TestMediaConfig::TestMediaConfig()
{
    MediaDevice vInput = MediaDevice("videotestsrc");
    vInput.addParameter(PayloadParameter("is-live", 1));
    vInput.setFilter("ffmpegcolorspace ! videoscale");
    setVideoInput(vInput);

    MediaDevice vOutput = MediaDevice("xvimagesink");
    vOutput.setFilter("ffmpegcolorspace ! videoscale");
    setVideoOutput(vOutput);
    setLocalVideoOutput(vOutput);

    MediaDevice audioInput = MediaDevice("audiotestsrc");
    audioInput.addParameter(PayloadParameter("is-live", 1));
    audioInput.addParameter(PayloadParameter("freq", 220));
    audioInput.setFilter("audioconvert ! audioresample");
    setAudioInput(audioInput);

    MediaDevice audioOutput = MediaDevice("pulsesink");
    audioOutput.setFilter("audioconvert ! audioresample");
    setAudioOutput(audioOutput);
    setRingOutput(audioOutput);
}
