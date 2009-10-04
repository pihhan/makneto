
#include <cstring>
#include <iostream>

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

        if (!m_source) {
            std::cerr << "GstInput element failed to create " << srcname << " element" << std::endl;
            return;
        }
    } else {
        std::cerr << "GstInput with empy source name!" << std::endl;
    }

}

void QGstInput::iterateSourcePads()
{
    gboolean done = FALSE;
    GstIterator * it;
    GstPad * pad = NULL;
    GstCaps * caps = NULL;
    gchar * caps_str = NULL;

    it = gst_element_iterate_src_pads(m_source);
    std::cout << "Iterating pads of source element" << std::endl;

    while(!done) {
        switch (gst_iterator_next(it, (void **) &pad)) {
            case GST_ITERATOR_OK:
                caps_str = "";
                if (gst_pad_is_linked(pad)) {
                    caps = gst_pad_get_negotiated_caps(pad);
                    caps_str = gst_caps_to_string(caps);
                }
                std::cout << "Pad name:\"" << gst_pad_get_name(pad) << "\" " 
                    << (gst_pad_is_linked(pad) ? "linked" : "") << " " 
                    << caps_str
                    << std::endl;
                g_free(caps_str);
                gst_object_unref(pad);
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            case GST_ITERATOR_ERROR:
                std::cerr << "Error on interating source element pads." << std::endl;
                done = TRUE;
                break;
            case GST_ITERATOR_DONE:
                done = TRUE;
                break;
        }
    }


}
