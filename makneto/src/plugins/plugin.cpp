#include "plugin.h"

#include <QtCore/QDebug>
#include <QtCore/QRect>

Plugin::Plugin(QGraphicsItem *graphicsItem) : m_graphicsItem(graphicsItem)
{
}

void Plugin::viewResized(const QSize &rect)
{
  qDebug() << "SCENE RECT CHANGED" << rect;
}

void Plugin::resized(const QSize &)
{
}
