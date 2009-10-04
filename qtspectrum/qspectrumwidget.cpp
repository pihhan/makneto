
#include <QPainter>
#include <QPoint>
#include <iostream>

#include "qspectrum.h"
#include "qspectrumwidget.h"

QSpectrumWidget::QSpectrumWidget ( QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f), m_maximum(0), m_minimum(-60), m_timer(parent)
{
    resize(400, 300);
    setMinimumSize(200,150);
    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()) );
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(printSpectrum()) );

    m_timer.start();
}

#if 0

void QSpectrumWidget::render(QPainter *painter, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags )
{
    unsigned int bands = m_spectrum->bands();
    unsigned blockwidth = size().width() / (bands+1) / 2;
    QRect block(0, size().height(), blockwidth, 0);

    const QSpectrum::FloatVector magnitudes = m_spectrum->magnitudes();

    painter->begin(this);
    painter->drawLine(0,0, size().width()-1, 0);
    painter->drawLine(0,0, 0, size().height()-1);

    for (unsigned int band = 0; band<bands; ++band) {
        unsigned int low = magnitudes[band];
        int blockheight = (-magnitudes[band]) * size().height() / (m_maximum-m_minimum);

        if (blockheight > size().height())
            blockheight = size().height();

        block.setX(band*2*blockwidth);
        block.setHeight(blockheight);
        painter->fillRect(block, painter->brush());
    }
    painter->end();
}
#endif


void QSpectrumWidget::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);

    unsigned int bands = m_spectrum->bands();
    unsigned blockwidth = size().width() / (bands+1) / 2;
    if (blockwidth < 1)
        blockwidth = 1;
    QRect block(0, 0, blockwidth, 0);

    const QSpectrum::FloatVector magnitudes = m_spectrum->magnitudes();

    painter->drawLine(0,0, size().width()-1, 0);
    painter->drawLine(0,0, 0, size().height()-1);

    QPoint previous(0,0);
    QBrush brush(QColor(150,150,150));

    for (unsigned int band = 0; band<bands; ++band) {
        unsigned int low = magnitudes[band];
        int blockheight = (-magnitudes[band]) * size().height() / (m_maximum-m_minimum);

        if (blockheight > size().height())
            blockheight = size().height();

        QPoint current(band* 2 * blockwidth, blockheight);

        block.setX(band*2*blockwidth);
        block.setHeight(blockheight);
    //    painter->fillRect(block, brush);
    //    painter->drawRect(block);

        painter->drawLine(previous, current);
        previous = current;
    }
    painter->end();
    delete painter;

}

/** @brief Set source spectrum class.
    It will also set minimum value from threshold of that spectrum.
*/
void QSpectrumWidget::setSpectrum(QSpectrum *spectrum)
{
    m_spectrum = spectrum;
    float x = spectrum->threshold();
    if (x < m_maximum)
        m_minimum = x;
}

void QSpectrumWidget::printSpectrum()
{
    const QSpectrum::FloatVector magnitudes = m_spectrum->magnitudes();

    for (size_t i=0; i< magnitudes.size(); ++i) {
        std::cout << magnitudes[i] << ", ";
    }
    std::cout << std::endl;
}

void QSpectrumWidget::setTimeout(int value)
{
    m_timer.setInterval(value);
}
