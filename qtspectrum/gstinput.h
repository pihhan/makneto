
#ifndef GSTINPUT_H
#define GSTINPUT_H


#include <gst/gst.h>


class QGstInput
{
    public:
    typedef enum { TYPE_ALSA, TYPE_OSS, TYPE_PULSE, TYPE_JACK } SourceTypes;

    QGstInput(SourceTypes type=TYPE_ALSA);

    GstElement *element()
    { return m_source; }

    protected:
    GstElement *m_source;
    SourceTypes m_type;

};

#endif
