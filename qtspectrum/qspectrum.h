

#ifndef QSPECTRUM_H
#define QSPECTRUM_H

#include <gst/gst.h>
#include <vector>
#include <stdint.h>

/** @brief Class representing spectrum meter values.
    It does contain spectrum elements for Gstreamer pipeline, but not graphical representation 
    of results.
    */
class QSpectrum
{
    public:
    typedef std::vector<float>  FloatVector;

    QSpectrum();

    static gboolean message_handler(GstBus *bus, GstMessage *message, gpointer data);

    void setBands(unsigned int bands);
    void setThreshold(float threshold);
    /** \brief Ustrednovaci koeficient, cim vetsi, tim vetsi vliv maji nove hodnoty. Od 0 do 1 */
    void setAlpha(float alpha);
    void setInterval(uint64_t interval);

    unsigned int bands();
    float threshold();
    float alpha() { return m_alpha; }
    uint64_t interval();

    GstElement * gstElement() { return m_spectrum; }

    FloatVector magnitudes() { return m_magnitudes; }
    FloatVector phases() { return m_phases; }

    /** \brief Add this object as watch of specified bus, so messages will flow to us. */
    void attachToBus(GstBus *bus);
    void attachToBus(GstBin *bin);
    
    void addToBin(GstElement *bin);
    bool linkFrom(GstElement *source);


    protected:
    void setValues(const GValue *magnitudes, const GValue *phases);

    private:
    GstElement *m_convert;
    GstElement *m_sink;
    GstElement *m_spectrum;
    GstBin      *bin;
    unsigned int m_bands;
    FloatVector m_magnitudes;
    FloatVector m_phases;
    float       m_alpha;


};

#endif

