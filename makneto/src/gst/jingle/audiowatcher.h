
#ifndef AUDIOWATCHER_H
#define AUDIOWATCHER_H

#include <list>
#include <gst/gst.h>

class AVOutput;

class GstAudioWatcher
{
    public:

    GstAudioWatcher();
    virtual ~GstAudioWatcher();

    void assignSource(GstElement *level);
    bool acceptsForElement(GstElement *level);
    virtual void updateMessage(GstMessage *msg) = 0;
    virtual void updateVolumes(int channels, double *rms, double *peak, double *decay) = 0;

    virtual void setAudioVolume(double volume);
    virtual double audioVolume() const;

    virtual void setMuted(bool muted);
    virtual bool muted() const;

    void setAudioOutput(AVOutput *audio);
    AVOutput * audioOutput() const;

    private:
    GstElement *m_level;
    AVOutput    *m_output;

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
