
#include "avoutput.h"
#include "gstvideowatcher.h"

GstVideoWatcher::GstVideoWatcher()
    : m_output(0)
{
}

GstVideoWatcher::~GstVideoWatcher()
{
}

void GstVideoWatcher::setOutput(AVOutput *output)
{
    m_output = output;
}

AVOutput *GstVideoWatcher::output() const
{
    return m_output;
}


