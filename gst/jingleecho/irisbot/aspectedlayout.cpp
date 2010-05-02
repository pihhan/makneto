
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QWidget>
#include <QWidgetItem>

#include <cmath>

#include "aspectedlayout.h"

AspectedLayout::AspectedLayout()
    : m_ratio(4.0/3.0), m_central(0), m_left(0), m_right(0), m_top(0), 
      m_bottom(0)
{
#if 0
    createBorderWidgets();
#endif
}

AspectedLayout::AspectedLayout(QWidget *parent)
    : QLayout(parent), m_ratio(4.0/3.0), m_central(0), m_left(0), 
      m_right(0), m_top(0), m_bottom(0)
{
#if 0
    createBorderWidgets();
#endif
}

void AspectedLayout::setCentralWidget(QWidget *widget)
{
    m_central = new QWidgetItem(widget);
}

void AspectedLayout::createBorderWidgets()
{
    m_top = new QWidgetItem(new QWidget());
    m_bottom = new QWidgetItem(new QWidget());
    m_left = new QWidgetItem(new QWidget());
    m_right = new QWidgetItem(new QWidget());
}

void AspectedLayout::addItem(QLayoutItem *item, Position pos)
{
    switch (pos) {
        case CENTER:
            m_central = item; 
            break;
        case LEFT:
            m_left = item;
            break;
        case RIGHT:
            m_right = item;
            break;
        case TOP:
            m_top = item;
            break;
        case BOTTOM:
            m_bottom = item;
            break;
    }
}

/** @brief Compute sizes of borders around center. */
void AspectedLayout::recomputeSizes(const QRect &current)
{
    double cur_ratio = ((double) current.width()) / current.height();

    double ratiodiff =  cur_ratio / m_ratio ;
    if (ratiodiff > 1) {
        // width is bigger than needed
        QSize top(current.width(), 0);

        QRect centerSize = m_central->geometry();
        centerSize.setWidth( ceil(m_ratio*current.height()) );
        centerSize.setHeight(current.height());

        int remains = current.width()- centerSize.width();
        int left = remains/2;
        centerSize.setX(current.x() + left);
        centerSize.setY(current.y());

        m_central->setGeometry(centerSize);
    } else if (ratiodiff < 1) {
        // height is bigger than needed
        QSize side(0, current.height());

        QRect centerSize = m_central->geometry();
        centerSize.setHeight( ceil(current.width() / m_ratio) );
        centerSize.setWidth(current.width());

        int remains = current.height() - centerSize.height();
        int top = remains/2;

        centerSize.setX(current.x());
        centerSize.setY(current.y() + top);
        m_central->setGeometry(centerSize);
    } else {
        m_central->setGeometry(current);
    }
}

void AspectedLayout::addItem(QLayoutItem *item)
{
    m_central = item;
}

int AspectedLayout::count() const
{
    int count = 0;
    if (m_central)
        count++;
    if (m_left)
        count ++;
    if (m_right)
        count++;
    if (m_top)
        count++;
    if (m_bottom)
        count++;
    return count;
}

QLayoutItem *AspectedLayout::itemAt(int index) const
{
    switch (index) {
        case 0:
        case 1: return m_central;
        case 2: return m_left;
        case 3: return m_right;
        case 4: return m_top;
        case 5: return m_bottom;
        default:
            return NULL;
    }
}

QLayoutItem *AspectedLayout::takeAt(int index)
{
    QLayoutItem *tmp;
    switch (index) {
        case 0:
        case 1: 
            tmp = m_central;
            m_central = 0;
            break;
        case 2: 
            tmp= m_left; 
            m_left = 0; break;
        case 3: 
            tmp = m_right; 
            m_right = 0; break;
        case 4: 
            tmp = m_top; 
            m_top = 0; break;
        case 5: 
            tmp = m_bottom; 
            m_bottom = 0; break;
        default:
            return NULL;
    }
    return tmp;
}

Qt::Orientations AspectedLayout::expandingDirections() const
{
    return (Qt::Vertical | Qt::Horizontal);
}
    
QSize AspectedLayout::sizeHint() const
{
    if (m_central)
        return m_central->sizeHint();
    else 
        return QSize(-1, -1);
}

void AspectedLayout::setRatio(int width, int height) 
{
    m_ratio = ((double) width) / height;
}

int AspectedLayout::indexOf(QWidget *widget) const
{
    if (m_central->widget() == widget)
        return 1;
    else if (m_left->widget() == widget)
        return 2;
    else if (m_right->widget() == widget)
        return 3;
    else if (m_top->widget() == widget)
        return 4;
    else if (m_bottom->widget() == widget)
        return 5;
    else return -1;
}

QSize AspectedLayout::maximumSize() const
{
    if (m_central)
        return m_central->maximumSize();
    else
        return QSize(-1, -1);
}

QSize AspectedLayout::minimumSize() const
{
    if (m_central)
        return m_central->minimumSize();
    else
        return QSize(-1, -1);

}

bool AspectedLayout::hasHeightForWidth() const
{
#if 0
    if (m_central)
        return m_central->hasHeightForWidth();
    else
        return false;
#else 
        return true;
#endif
}

int AspectedLayout::heightForWidth(int w) const
{
    if (m_central) {
        int height = ceil(w / m_ratio);
        return height;
    } else {
        return -1;
    }
}

void AspectedLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);

    if (m_central)
        recomputeSizes(rect);

}

