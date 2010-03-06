
#include "logger/logger.h"
#include "avoutput.h"

bool AVOutput::enableAudio()
{
}

bool AVOutput::enableVideo()
{
}

bool AVOutput::audioEnabled()
{
}
bool AVOutput::videoEnabled()
{
}

MediaDevice AVOutput::videoConfig() const
{
}
MediaDevice AVOutput::audioConfig() const
{
}

void AVOutput::setVideoConfig(const MediaDevice &d)
{
}
void AVOutput::setAudioConfig(const MediaDevice &d)
{
}

GstPad AVOutput::*getVideoSinkPad()
{
}
GstPad AVOutput::*getAudioSinkPad()
{
}

GstElement *AVOutput::videoElement()
{
}
GstElement *AVOutput::audioElement()
{
}
GstElement *AVOutput::videoFilterElement()
{
}
GstElement *AVOutput::audioFilterElement()
{
}

std::string AVOutput::name()
{
}
void AVOutput::setName(const std::string &name)
{
}

