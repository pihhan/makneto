
#ifndef VOLUMEBAR_H
#define VOLUMEBAR_H

#include <QWidget>
#include <QFrame>
#include <QVector>
#include <QPaintEvent>
#include <QColor>

#include <gst/gst.h>
#include "audiowatcher.h"

/** @brief Volume bar widget for displaying data from Gstreamer "level" element. */
class VolumeBar : public QFrame, public GstAudioWatcher
{
    Q_OBJECT
    public:
    typedef QVector<double> DoubleVector;
    class Private;

    VolumeBar(QWidget *parent = 0);
    virtual ~VolumeBar();

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation o);

    double minimum() const;
    double maximum() const;
    void setMinimum(double minimum);
    void setMaximum(double maximum);

    double value(int channel = 0) const;
    void setValue(double value, int channel = 0);

    double peak(int channel = 0) const;
    void setPeak(double peak, int channel = 0);

    double decay(int channel = 0) const;
    void setDecay(double decay, int channel = 0);

    int channels() const;
    void setChannels(int channels);

    bool autorange() const;
    void setAutorange(bool ar);

    virtual QSize sizeHint() const;

    void setBlockDelimColor(const QColor &color);
    void setLevelColor(const QColor &color);
    void setPeakColor(const QColor &color);

    void registerSource(GstElement *pipeline, GstElement *level);
    virtual void updateMessage(GstMessage *msg);

    Q_SIGNALS:
    void volumeChanged();

    protected:

    virtual void paintEvent(QPaintEvent *event);
    void dataChanged();

    private:

    Qt::Orientation m_orientation;
    int     m_channels;
    DoubleVector    m_values;
    DoubleVector    m_peaks;
    DoubleVector    m_decays;
    double  m_min;
    double  m_max;
    bool    m_autorange;
    QColor  m_blockDelimColor;
    QColor  m_levelColor;
    QColor  m_peakColor;
    Private *prv;

};

#endif

