/* Graficke znazorneni spekralni analyzy.
 Widget, ktery periodicky prekresluje hodnotami pocitanymi z
 message. */

#ifndef QSPECTRUMWIDGET_H
#define QSPECTRUMWIDGET_H

#include <QWidget>
#include <QTimer>

#include "qspectrum.h"


class QSpectrumWidget : public QWidget
{
    Q_OBJECT

    public:
    QSpectrumWidget ( QWidget *parent = 0, Qt::WindowFlags f = 0);

//    virtual void render(QPainter *painter, const QPoint & targetOffset = QPoint(), const QRegion & sourceRegion = QRegion(), RenderFlags renderFlags = RenderFlags( DrawWindowBackground | DrawChildren ) );

    QSpectrum * spectrum() { return m_spectrum; }
    void setSpectrum(QSpectrum *spectrum);
    void setTimeout(int timeout);

    public slots:
    void printSpectrum();

    protected:
    void paintEvent(QPaintEvent *event);
        
    private:
    QSpectrum *m_spectrum;
    float   m_maximum;
    float   m_minimum;
    QTimer  m_timer;
};

#endif

