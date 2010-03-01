
#include "testmediaconfig.h"

/** @brief Create configuration for testing. 
    That means automatic generator of video and audio signal,
    and output to user display. */
TestMediaConfig::TestMediaConfig()
{
    m_videoInput = MediaDevice("videotestsrc");
    m_videoOutput = MediaDevice("ximagesink");
    m_localVideoOutput = MediaDevice("ximagesink");
    m_audioInput = MediaDevice("audiotestsrc");
    m_audioInput.addParameter(PayloadParameter("is-live", 1));
    m_audioInput.addParameter(PayloadParameter("freq", 220));
    m_audioOutput = MediaDevice("pulsesink");
    m_ringOutput = MediaDevice("pulsesink");
}
