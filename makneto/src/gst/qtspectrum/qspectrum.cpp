
#include <iostream>
#include <cstring>
#include <stdint.h>
#include "qspectrum.h"
#include <QDebug>


QSpectrum::QSpectrum()
    : m_alpha(0.3)
{

    m_sink = gst_element_factory_make("fakesink", "sink");
    m_convert = gst_element_factory_make("audioconvert", "convert");
    m_spectrum = gst_element_factory_make("spectrum", "spectrum");

    if (!m_sink || !m_convert || !m_spectrum) {
        qWarning("Some gstreamer modules were not created!");
        return;
    }

    setBands(20);

}

/** \brief Zpracuje zpravu z Gstreameru s hodnotami ze spektralni analyzy. 
    \param data Ukazatel na tridu QSpectrum. */
gboolean QSpectrum::message_handler(GstBus *bus, GstMessage *message, gpointer data)
{
    if (message->type == GST_MESSAGE_ELEMENT) {
        const GstStructure *s = gst_message_get_structure(message);
        const gchar *name = gst_structure_get_name(s);

        if (!strcmp(name, "spectrum")) {
            const GValue *magnitudes = gst_structure_get_value(s, "magnitude");
            const GValue *phases = gst_structure_get_value(s, "phase");
            QSpectrum *spectrum = static_cast<QSpectrum *>(data);
            if (spectrum)
                spectrum->setValues(magnitudes, phases);
        }
#ifdef DEBUG
        std::cout << "gst bus message: " << name << std::endl;
#endif
        }
    // FIXME: co vlastne znamena navratova hodnota?
    return true;
}

/** \brief Nastavi hodnoty obrzene ve zprave do vnitrni struktury objektu. */
void QSpectrum::setValues(const GValue *magnitudes, const GValue *phases)
{
    int bands = gst_value_list_get_size(magnitudes);
    if (m_magnitudes.size() != (unsigned int) bands)
        m_magnitudes.resize(bands, 0);
    if (m_phases.size() != (unsigned int) bands)
        m_phases.resize(bands, 0);

    float alpha_complement = 1- m_alpha;

    for (int i=0; i<bands; ++i) {
        const GValue *one_magnitude = gst_value_list_get_value(magnitudes, i);
        m_magnitudes[i] = m_magnitudes[i]*alpha_complement + m_alpha*g_value_get_float(one_magnitude);
#ifdef READ_PHASES
        const GValue *one_phase = gst_value_list_get_value(phases, i);
        m_phases[i] = m_phases[i]*alpha_complement + m_alpha*g_value_get_float(one_phase);
#endif // READ_PHASES
    }
}


void QSpectrum::setBands(unsigned int bands)
{
    m_magnitudes.resize(bands, 0);
    m_phases.resize(bands, 0);
    m_bands = bands;
    g_object_set( G_OBJECT(m_spectrum), "bands", bands, NULL );
}

void QSpectrum::setThreshold(float threshold)
{
    g_object_set( G_OBJECT(m_spectrum), "threshold", threshold, NULL);
}

unsigned int QSpectrum::bands()
{
    return m_bands;
}

float QSpectrum::threshold()
{
    float threshold = -1;
    g_object_get( G_OBJECT(m_spectrum), "threshold", &threshold, NULL);
    return threshold;
}

void QSpectrum::setAlpha(float alpha)
{
    m_alpha = alpha;
}

void QSpectrum::setInterval(uint64_t interval)
{
    g_object_set( G_OBJECT(m_spectrum), "interval", interval, NULL);
}

uint64_t QSpectrum::interval()
{
    uint64_t interval;
    g_object_get( G_OBJECT(m_spectrum), "interval", &interval, NULL);
    return interval;
}

void QSpectrum::attachToBus(GstBus *bus)
{
    gst_bus_add_watch(bus, QSpectrum::message_handler, this);
}

void QSpectrum::addToBin(GstElement *bin)
{
    GstBus *bus = gst_element_get_bus(bin);
    gst_bus_add_watch(bus, QSpectrum::message_handler, this);
    g_object_unref(bus);
    GstBin *gbin = GST_BIN(bin);

    gst_bin_add(gbin, m_convert);
    gst_bin_add(gbin, m_spectrum);
    gst_bin_add(gbin, m_sink);

    bool convert_linked = gst_element_link(m_convert, m_spectrum);
    bool spectrum_linked = gst_element_link(m_spectrum, m_sink);
    if(!convert_linked || !spectrum_linked) {
        qWarning("Problem with linking convert, spectrum or sink elements!");
        return;
    }
}

bool QSpectrum::linkFrom(GstElement *source)
{
    return gst_element_link(source, m_convert);

}


