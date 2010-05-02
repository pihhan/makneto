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


#include "player.h"
#include "playerwidgetitem.h"
#include "volumebar.h"

//#include <QListWidget>


Player::Player(QWidget *parent) 
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

void Player::displayHandle()
{
    qDebug() << "Player handle is " << x11PictureHandle() << endl;
}
        
void Player::setVideoSize(const QSize size)
{
    m_videoSize = size;
    updateGeometry();
}

QSize Player::videoSize() const
{
    return m_videoSize;
}

QSize Player::sizeHint() const
{
    return m_videoSize;
}

QSize Player::minimumSizeHint() const
{
    return QSize(160,120);
}

bool Player::playing() const
{
    return m_playing;
}

void Player::setPlaying(bool playing)
{
    m_playing = playing;
}

void Player::paintEvent(QPaintEvent *event)
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

