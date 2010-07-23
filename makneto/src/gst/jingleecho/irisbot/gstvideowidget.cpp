
#include <cmath>

#include <Qt>
#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QFileDialog>
#include <QDebug>

#include <QPaintEngine>
#include <QPaintEvent>
#include <QPainter>
#include <QPaintDevice>

#include <QX11Info>
#include <X11/X.h>
#include <X11/Xlib.h>


//#include "player.h"
//#include "playerwidgetitem.h"
#include "volumebar.h"
#include "gstvideowidget.h"

//#include <QListWidget>


GstVideoWidget::GstVideoWidget(QWidget *parent) 
    : QWidget(parent), m_playing(false), m_keepAspectRatio(true),
      m_zoom(1.0), m_resizable(true)
{
    m_videoSize = QSize(320,240);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
//    setMinimumSize(320,240);
//  setMaximumSize(320,240);

    QPalette bgpal(palette());
    bgpal.setColor(QPalette::Active, QPalette::Background, QColor(Qt::black));
    setPalette(bgpal);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_PaintOnScreen);
}
        
GstVideoWidget::~GstVideoWidget() 
{
}

void GstVideoWidget::displayHandle()
{
    qDebug() << "Player handle is " << winId() << endl;
}
        
void GstVideoWidget::setVideoSize(const QSize size)
{
    m_videoSize = size;
    updateGeometry();
}

QSize GstVideoWidget::videoSize() const
{
    return m_videoSize;
}

QSize GstVideoWidget::sizeHint() const
{
    QSize size(m_zoom * m_videoSize.width(), m_zoom * m_videoSize.height());
    return size;
}

QSize GstVideoWidget::minimumSizeHint() const
{
    return QSize(160,120);
}

bool GstVideoWidget::playing() const
{
    return m_playing;
}

void GstVideoWidget::setPlaying(bool playing)
{
    m_playing = playing;
}

void GstVideoWidget::paintEvent(QPaintEvent *event)
{
    if (!m_playing)
#if 0
        QWidget::paintEvent(event);
    else {
#else 
    {
#endif
        event->accept();

        QPainter painter(this);
        //engine.begin(this);
        painter.eraseRect(event->rect());
        //engine.end();
    } else {
        event->ignore();
    }
}

/** \brief Enforce aspect ratio for video widget, if keeping aspect ratio 
    is true. Width is used to get new size, height is computed to match. */
void GstVideoWidget::resizeEvent(QResizeEvent *event)
{
    double rate = (double) event->size().width() / event->oldSize().width();
    int newheight = ceil(event->oldSize().height() * rate);

    if (m_keepAspectRatio && newheight != event->size().height()) {
        // FIXME: use heightForWidth and hope it works
        //setHeight(newheight);
        event->accept();
    } else
        event->ignore();
}

/** \brief Compute preferred height for passed width, when keeping aspect ratio.
*/
int GstVideoWidget::heightForWidth(int w) const
{
    if (m_keepAspectRatio) {
        double ratio = m_videoSize.height() / m_videoSize.width();
        return ceil(w * ratio);
    } else {
        return -1;
    }
}

bool GstVideoWidget::keepAspectRatio() const
{
    return m_keepAspectRatio;
}

void GstVideoWidget::setKeepAspectRatio(bool keep)
{
    m_keepAspectRatio = keep;
}

double GstVideoWidget::zoom() const
{
    return m_zoom;
}

void GstVideoWidget::setZoom(double zoom)
{
    m_zoom = zoom;
    updateGeometry();
}

bool GstVideoWidget::resizable() const
{
    return m_resizable;
}

void GstVideoWidget::setResizable(bool is_resizable)
{
    m_resizable = is_resizable;
}

