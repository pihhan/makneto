
#include <cstring>
#include "gstinput.h"


QGstInput::QGstInput(SourceTypes type)
{
    const char *srcname = NULL;
    switch (type) {
        case TYPE_ALSA:     srcname = "alsasrc"; break;
        case TYPE_OSS:      srcname = "osssrc"; break;
        case TYPE_PULSE:    srcname = "pulsesrc"; break;
        case TYPE_JACK:     srcname = "jackaudiosrc"; break;
    }

    if (srcname) {
        m_source = gst_element_factory_make(srcname, "input");
        m_type = type;
    }

}

