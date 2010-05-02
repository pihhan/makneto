
#include "logger/logger.h"
#include "avoutputmanager.h"

AVOutputManager::AVOutputManager()
{
}

void AVOutputManager::addOutput(AVOutput *out)
{
    m_outputs.push_back(out);
    registerAllVideoElements(out);
    registerAllAudioElements(out);
}

void AVOutputManager::removeOutput(AVOutput *out)
{
    unregisterAllVideoElements(out);
    unregisterAllAudioElements(out);
    for (OutputList::iterator it=m_outputs.begin(); it!=m_outputs.end(); it++) {
        if (*it == out)
            m_outputs.erase(it);
    }
}

/** @brief Find output structure by element it contains.
    @param element Element contained inside structure to lookup.
    It will search output database by unique name every element in pipeline
    has. */
AVOutput *AVOutputManager::findOutput(GstElement *element)
{
    return findOutput(GST_OBJECT(element));
}

/** @brief Find output structure by element it contains.
    @param element Element contained inside structure to lookup.
    It will search output database by unique name every element in pipeline
    has. */
AVOutput *AVOutputManager::findOutput(GstObject *object)
{
    const gchar *name = gst_object_get_name(object);
    if (!name)
        return NULL;
    OutputStringMap::iterator it = m_elementsMap.find(name);
    if (it != m_elementsMap.end())
        return it->second;
    else
        return NULL;
}

AVOutput *AVOutputManager::findOutput(const std::string &participant)
{
    for (OutputList::const_iterator it=m_outputs.begin(); it!= m_outputs.end(); it++) {
        if ((*it)->name() == participant)
            return *it;
    }
    return NULL;
}



/** @brief Adds element to internal list of elements.
    It will also unref element, to make code nicer at output adding. */
void AVOutputManager::registerElement(GstElement *element, AVOutput *output)
{
    if (!element)
        return;
    gchar *name = gst_object_get_name(GST_OBJECT(element));
    m_elementsMap.insert(std::make_pair(name, output));
    g_free(name);
    gst_object_unref(GST_OBJECT(element));
}

/** @brief Removes element from internal list of elements. 
    It will also unref element, to make code nicer at output removing. */
void AVOutputManager::unregisterElement(GstElement *element)
{
    if (!element)
        return;
    gchar *name = gst_object_get_name(GST_OBJECT(element));
    m_elementsMap.erase(name);
    g_free(name);
    gst_object_unref(GST_OBJECT(element));
}

void AVOutputManager::registerAllVideoElements(AVOutput *out)
{
    registerElement(out->videoElement(), out);
}

void AVOutputManager::unregisterAllVideoElements(AVOutput *out)
{
    unregisterElement(out->videoElement());
}

void AVOutputManager::registerAllAudioElements(AVOutput *out)
{
    registerElement(out->audioElement(), out);
    registerElement(out->levelElement(), out);
    registerElement(out->volumeElement(), out);
}

void AVOutputManager::unregisterAllAudioElements(AVOutput *out)
{
    unregisterElement(out->audioElement());
    unregisterElement(out->levelElement());
    unregisterElement(out->volumeElement());
}

bool AVOutputManager::handleLevelMessage(GstMessage *msg)
{
    AVOutput *out = findOutput(msg->src);
    if (out) {
        return out->handleLevelMessage(msg);
    } else {
        LOGGER(logit) << "AVOutput not found for message" << std::endl;
    }
    return false;
}


