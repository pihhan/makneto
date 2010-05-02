
#ifndef AUDIOWATCHER_H
#define AUDIOWATCHER_H

#include <list>
#include <gst/gst.h>

class GstAudioWatcher
{
    public:

    GstAudioWatcher();
    void assignSource(GstElement *level);
    bool acceptsForElement(GstElement *level);
    virtual void updateMessage(GstMessage *msg) = 0;

    private:
    GstElement *m_level;

};

typedef std::list<GstAudioWatcher *> GstAudioWatcherList;

class GstAudioWatcherRegistry
{
    public:
    GstAudioWatcherRegistry();

    void registerWatcher(GstAudioWatcher *watcher, GstElement *level);
    void propagateLevelMessage(GstMessage *msg);

    GstAudioWatcherList watchers;
};

#endif
