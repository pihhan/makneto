
#include "qspectrum.h"
#include "qspectrumwidget.h"

QSpectrumWidget::QSpectrumWidget ( QWidget *parent = 0, Qt::WindowFlags f = 0)
    : QWidget(parent, f), m_maximum(0), m_minimum(-90), m_timer(parent)
{
    resize(400, 300);
    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()) );
    m_timer.start();
}

virtual void QSpectrumWidget::render(QPainter *painter, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags )
{
    unsigned int bands = m_spectrum.bands();
    unsigned blockwidth = size().width() / (bands+1) / 2;
    QRect block(0, size().height(), blockwidth, 0);

    const QSpectrum::FloatVector magnitudes = m_spectrum.magnitudes();

    for (unsigned int band = 0; band<bands; ++band) {
        unsigned int low = magnitudes[band];
        int blockheight = (-magnitudes[band]) * size().height() / (m_maximum-m_minimum);
        block.setX(2*blockwidth);
        block.setHeight(blockheight);
        painter->fillRect(block, brush());
    }
}

