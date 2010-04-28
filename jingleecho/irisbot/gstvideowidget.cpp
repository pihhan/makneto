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
    : QWidget(parent), m_playing(false)
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
    return m_videoSize;
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

        QPainter *painter = new QPainter(this);
        //engine->begin(this);
        painter->eraseRect(event->rect());
        //engine->end();

        delete painter;

        return;
    }
    event->ignore();
}

