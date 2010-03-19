
#ifndef AVOUTPUTMANAGER_H
#define AVOUTPUTMANAGER_H

#include <map>
#include <list>
#include <string>

#include <gst/gst.h>

#include "avoutput.h"

/** @brief Class for managing list of av outputs.
    @author Petr Mensik <pihhan@cipis.net>

    It is able to search output by name of gstreamer element or 
    other parameters. It should also handle some messages from pipeline
    and forward parameters to output dependent functions.
*/
class AVOutputManager
{
    public:
    typedef std::list<AVOutput *>     OutputList;
    typedef std::map<std::string, AVOutput *>   OutputStringMap;

    AVOutputManager();

    void addOutput(AVOutput *out);
    void removeOutput(AVOutput *out);

    AVOutput *findOutput(GstElement *element);
    AVOutput *findOutput(const std::string &participant);

    void registerElement(GstElement *element, AVOutput *output);
    void unregisterElement(GstElement *element);

    bool handleLevelMessage(GstMessage *msg);

    OutputStringMap     m_elementsMap;
    OutputList          m_outputs;
};

#endif


