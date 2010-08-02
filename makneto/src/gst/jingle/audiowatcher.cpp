/*
 *
 * AudioWatcher
 * Structure for listening to volume levels without 
 * integrating GUI and Gstreamer too much.
 *
 */

#include "audiowatcher.h"
#include "avoutput.h"

GstAudioWatcher::GstAudioWatcher()
    : m_level(0)
{
}


GstAudioWatcher::~GstAudioWatcher()
{
}

void GstAudioWatcher::assignSource(GstElement *level)
{
    if (m_level)
        gst_object_unref(GST_OBJECT(m_level));
    gst_object_ref(GST_OBJECT(level));
    m_level = level;
}

bool GstAudioWatcher::acceptsForElement(GstElement *level)
{
    return (!m_level || m_level == level);
}

void GstAudioWatcher::setAudioOutput(AVOutput *output)
{
    m_output = output;
}

AVOutput * GstAudioWatcher::audioOutput() const
{
    return m_output;
}

void GstAudioWatcher::setAudioVolume(double volume)
{
    if (m_output)
        m_output->setVolume(volume);
}

double GstAudioWatcher::audioVolume() const
{
    if (m_output)
        return m_output->volume();
    else
        return 0.0;
}


void GstAudioWatcher::setMuted(bool muted)
{
    if (m_output)
        m_output->setMuted(muted);
}

bool GstAudioWatcher::muted() const
{
    if (m_output)
        return m_output->muted();
    else
        return true;
}


/***
 * GstWatcherRegistry ***i
 **/

GstAudioWatcherRegistry::GstAudioWatcherRegistry()
{
}

void GstAudioWatcherRegistry::registerWatcher(GstAudioWatcher *watcher, GstElement *level)
{
    watcher->assignSource(level);
    watchers.push_back(watcher);
}

/** @brief Deliver message with update to all interested. */
void GstAudioWatcherRegistry::propagateLevelMessage(GstMessage *msg)
{
    GstAudioWatcherList::const_iterator it;
    for (it=watchers.begin(); it!=watchers.end(); it++) {
        GstElement *e = GST_ELEMENT(msg->src);
        if ((*it)->acceptsForElement(e)) 
            (*it)->updateMessage(msg);
    }
}


