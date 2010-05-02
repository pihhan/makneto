/*
 * Zapouzdreni elementu pro C++ / Qt 
 */
#ifndef QGSTELEMENT_H
#define QGSTELEMENT_H

#include <gst/gst.h>

class QGstElement 
{
    public:
    /** \brief Zinicializuj strukturu z existujiciho elementu. */
    QGstElement(GstElement *element);

    /** \brief Vytvor novy element pomoci jmena z factory. */
    QGstElement(const char *element_type, const char *element_name = NULL);

    private:
    GstElement *element;
};

#endif
