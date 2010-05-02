#ifndef GSTTEST_H
#define GSTTEST_H

#include <vector>
#include <gst/gst.h>

#include "audiowatcher.h"

class GstTest
{
    public:
    GstTest(bool test=false);

    void createPipeline();
    void createTestPipeline();
    void createVideoPipeline();
    void createAudioPipeline();

    void play();
    void pause();
    void stop();

    void togglePlay();

    void setLocation(const char *path);

    void stopped();

    /** \brief callback pro zpracovavani zprav. */
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

    /** \brief Pripoj nove objeveny stream do dekoderu */
    void connectDecoder(GstPad *pad);

    static void pad_watch(GstElement *element, GstPad *pad, gpointer data);

    void handleError(GstMessage *msg);

    static void initialize(int argc, char *argv[]);

    void setXID(long xid) { xwindowid = xid; }

    void configureOverlay();
    void printSourcePads();
    std::vector<int> getResolutions(GstCaps *caps);
    std::vector<int> getResolutions();
    
    void setResolution(int w, int h);
    int negotiatedHeight();
    int negotiatedWidth();
    void enumerateDevices(GstElement *source);

//  private:
        GstElement *pipeline;
        GstElement *source;
        GstElement *parser;
        GstElement *decoder;
        GstElement *decoder2;
        GstElement *convert;
        GstElement *sink;
        GstElement *level;
        GstElement *volume;
        GstElement *audiosrc;
        GstElement *audiosink;
        GstBus *bus;

        GstAudioWatcherRegistry watchers;
    private:
        gboolean playing;
        long xwindowid;

};



#endif
