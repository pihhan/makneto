
#include <iostream>
#include <QVector>
#include <QPainter>

#include "volumebar.h"
#include <gst/gst.h>

class VolumeBar::Private
{
    /*
    Class for handling gstreamer part of getting gstreamer elements,
    and hiding most of this from user. */
    public:
    Private(VolumeBar *bar)
    {
        this->bar = bar;
        watchId = 0;
        source = 0;
    }

    bool parseStructure(GstStructure *structure);
    void registerPipelineHandler(GstElement *pipeline, GstElement *source = NULL);
    void registerBusHandler(GstBus *bus, GstElement *source = NULL);
    static gboolean bus_call(GstBus *, GstMessage *msg, gpointer data);
    void updateMessage(GstMessage *msg);

    private:
    guint watchId;
    VolumeBar *bar;
    GstObject *source;
};




VolumeBar::VolumeBar(QWidget *parent)
    : QFrame(parent), m_orientation(Qt::Horizontal), m_channels(1), 
      m_min(-70), m_max(-50), m_autorange(true)
{
    prv = new Private(this);
    setChannels(m_channels);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    setMinimumSize(100, 20);


    m_blockDelimColor = QColor( Qt::darkGray );
    m_levelColor = QColor( Qt::yellow );
    m_peakColor = QColor( Qt::red );

    connect(this, SIGNAL(volumeChanged()), this, SLOT(update()) );

    setFrameShadow(QFrame::Sunken);
    setFrameShape(QFrame::StyledPanel);
}

VolumeBar::~VolumeBar()
{
    delete prv;
}

Qt::Orientation VolumeBar::orientation() const
{
    return m_orientation;
}

void VolumeBar::setOrientation(Qt::Orientation o)
{
    if (o == Qt::Vertical)
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    else
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    m_orientation = o;
}

double VolumeBar::minimum() const
{
    return m_min;
}

double VolumeBar::maximum() const
{
    return m_max;
}

void VolumeBar::setMinimum(double minimum)
{
    m_min = minimum;
}

void VolumeBar::setMaximum(double maximum)
{
    m_max = maximum;
}

double VolumeBar::value(int channel) const
{
    if (channel < m_values.size())
        return m_values[channel];
    else 
        return -1;
}

void VolumeBar::setValue(double value, int channel)
{
    if (channel < m_values.size())
        m_values[channel] = value;
}

double VolumeBar::peak(int channel) const
{
    if (channel < m_peaks.size())
        return m_peaks[channel];
    else
        return -1;
}

void VolumeBar::setPeak(double peak, int channel)
{
    if (channel < m_peaks.size())
        m_peaks[channel] = peak;
}

int VolumeBar::channels() const
{
    return m_channels;
}

void VolumeBar::setChannels(int channels)
{
    m_channels = channels;
    m_values.resize(channels);
    m_peaks.resize(channels);
    m_decays.resize(channels);
}

/** @brief Implemented drawing of bar itself. */
void VolumeBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setBrush( Qt::SolidPattern );

    QRect geo = geometry();

    int borderwidth = 4;
    int heightSpaces = (m_channels+1) * borderwidth;
    int meterHeight = (geo.height() - heightSpaces) / m_channels;
    int blockCount = 10;
    int valuePixels = (geo.width()-(2*borderwidth));
    int blockStep = (valuePixels) / blockCount;

    double valueRange = m_max - m_min;
    
    for (int channel=0; channel < m_channels; channel++) {
        double value = m_values[channel];
        double peak = m_peaks[channel];
        double decay = m_decays[channel];
        if (value < m_min)
            value = m_min;
        if (value > m_max) {
            if (m_autorange)
                m_max = value;
            else
                value = m_max;

        }
        if (peak < m_min)
            peak = m_min;
        if (peak > m_max) {
            if (m_autorange)
                m_max = peak;
            else
                peak = m_max;
        }

        if (decay < m_min)
            decay = m_min;
        if (decay > m_max)
            decay = m_max;

        int highoffset = channel * (meterHeight+borderwidth) + borderwidth;

        double percent = (value-m_min)/ valueRange;
        int pixels = percent * valuePixels;
        QRect box(borderwidth, highoffset, pixels, meterHeight);
        painter.setPen(m_levelColor);
        painter.fillRect(box, m_levelColor);

        int peakpixels = (peak-m_min)/valueRange * valuePixels;
        if (peakpixels >= 0 && peakpixels <= geo.width()) {
            QPoint pfrom(peakpixels+borderwidth, highoffset);
            QPoint pto(pfrom.x(), highoffset+meterHeight);
            painter.setPen(m_peakColor);
            painter.drawLine(pfrom, pto);
        }

        int decaypixels = (decay-m_min) /valueRange * valuePixels;
        QPoint pfrom(decaypixels+borderwidth, highoffset);
        QPoint pto(pfrom.x(), highoffset+meterHeight);
        painter.setPen(m_blockDelimColor);
        painter.drawLine(pfrom, pto);
    }

    painter.setPen(m_blockDelimColor);
    QRect workarea = contentsRect();
    for (int i=0; i<blockCount; i++) {
        int x = i*blockStep+borderwidth;
        QPoint from(x, workarea.x());
        QPoint to(x, workarea.bottom());
        painter.drawLine(from, to);
    }

    event->accept();
    QFrame::paintEvent(event);
}

QSize VolumeBar::sizeHint() const
{
    if (m_orientation == Qt::Vertical)
        return QSize(10, 20);
    else
        return QSize(20, 10);
}

void VolumeBar::setBlockDelimColor(const QColor &color)
{
    m_blockDelimColor = color;
}
void VolumeBar::setLevelColor(const QColor &color)
{
    m_levelColor = color;
}
void VolumeBar::setPeakColor(const QColor &color)
{
    m_peakColor = color;
}

double VolumeBar::decay(int channel) const
{
    if (channel < m_decays.size() && channel >= 0)
        return m_decays[channel];
    else
        return -1;
}
    
void VolumeBar::setDecay(double decay, int channel)
{
    if (channel < m_decays.size() && channel >= 0)
        m_decays[channel] = decay;
}

/** @brief Register source element for this widget. 
    @param pipeline Element of pipeline for getting its bus.
    @param level Element level that we listen for. */
void VolumeBar::registerSource(GstElement *pipeline, GstElement *level)
{
    prv->registerPipelineHandler(pipeline, level);
}

void VolumeBar::dataChanged()
{
    emit volumeChanged();
    std::cout << "Volume channel 1: " << m_values[0] << std::endl;
}

void VolumeBar::updateMessage(GstMessage *msg)
{
    prv->updateMessage(msg);
}

bool VolumeBar::autorange() const
{
    return m_autorange;
}

void VolumeBar::setAutorange(bool ar)
{
    m_autorange = ar;
}

/*
 *
 * Private part
 *
 */

void VolumeBar::Private::registerPipelineHandler(GstElement *pipeline, GstElement *source)
{
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    if (bus) {
        registerBusHandler(bus, source);
        gst_object_unref(GST_OBJECT(bus));
    }
}

void VolumeBar::Private::registerBusHandler(GstBus *, GstElement *source)
{
    if (source)
        this->source = GST_OBJECT(source);
    else
        this->source = NULL;
//    watchId = gst_bus_add_watch(bus, bus_call, this);
}

bool VolumeBar::Private::parseStructure(GstStructure *s)
{
    if (s && gst_structure_has_name(s, "level")) {
        const GValue *peaks = gst_structure_get_value(s, "peak");
        const GValue *decays = gst_structure_get_value(s, "decay");
        const GValue *levels = gst_structure_get_value(s, "rms");

#if 0
        memset(&peaks,  0, sizeof(peaks));
        g_value_init(&peaks, GST_TYPE_LIST);
        memset(&decays, 0, sizeof(decays));
        g_value_init(&decays, GST_TYPE_LIST);
        memset(&levels, 0, sizeof(levels));
        g_value_init(&levels, GST_TYPE_LIST);

        if (!gst_structure_get(s, 
            "peak", GST_TYPE_LIST, &peaks,
            "decay", GST_TYPE_LIST, &decays,
            "rms", GST_TYPE_LIST, &levels,
            NULL)) {
            g_error("Structure with volume level has invalid format.");
            return false;
        } else {
#else
        if (peaks && decays && levels) {
#endif
            for (unsigned int channel=0; 
                    channel < gst_value_list_get_size(peaks) ;
                    channel++) {
                const GValue *v = gst_value_list_get_value(peaks, channel);
                bar->setPeak(g_value_get_double(v), channel);
            }

            for (unsigned int channel=0; 
                    channel < gst_value_list_get_size(decays);
                    channel++) {
                const GValue *v = gst_value_list_get_value(decays, channel);
                bar->setDecay(g_value_get_double(v), channel);
            }

            for (unsigned int channel=0; 
                    channel < gst_value_list_get_size(levels);
                    channel++) {
                const GValue *v = gst_value_list_get_value(levels, channel);
                bar->setValue(g_value_get_double(v), channel);
            }

            bar->dataChanged();

        }
    }
    return true;
}

void VolumeBar::Private::updateMessage(GstMessage *msg)
{
    if (msg 
        && GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ELEMENT 
        && (!source || msg->src == source)
        && msg->structure
        && gst_structure_has_name(msg->structure, "level")) {

        parseStructure(msg->structure);
    }

}


gboolean VolumeBar::Private::bus_call(GstBus * /*bus*/, GstMessage *msg, gpointer data)
{
    g_print("Got volumebar message.");
    VolumeBar::Private *prv = static_cast<VolumeBar::Private *> (data);
    VolumeBar *bar = NULL;
    if (prv)
        bar = prv->bar;

    if (!msg 
        || !bar
        || GST_MESSAGE_TYPE(msg) != GST_MESSAGE_ELEMENT 
        || (prv->source && msg->src != prv->source))
        return true;

    if (msg->structure && gst_structure_has_name(msg->structure, "level")) {
            prv->parseStructure(msg->structure);
    }

    return true;
}

/** @brief Update volumes for this bar. */
void VolumeBar::updateVolumes(int channels, 
    double *rms, 
    double *peaks, 
    double *decays)
{
    for (int channel = 0; channel < channels; channel++) {
        if (rms)
            m_values[channel] = rms[channel];
        if (peaks)
            m_peaks[channel] = peaks[channel];
        if (decays)
            m_decays[channel] = decays[channel];
    }
    dataChanged();
}

