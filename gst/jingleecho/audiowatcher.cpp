/*
 *
 * AudioWatcher
 * Structure for listening to volume levels without 
 * integrating GUI and Gstreamer too much.
 *
 */

#include "audiowatcher.h"

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


