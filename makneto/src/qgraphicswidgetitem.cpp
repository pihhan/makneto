/****************************************************************************
**
** Copyright (C) 2007-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Graphics Widget Items project on Trolltech Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
 
#include "qgraphicswidgetitem.h"

#include <QtGui/qevent.h>
#include <QtGui/QGraphicsView>

QGraphicsWidgetItem::QGraphicsWidgetItem(QGraphicsWidgetItem *parent)
    : QGraphicsItem(parent, 0), embeddedWidget(0), lastActiveView(0)
{
    setFlag(ItemIgnoresTransformations);
}

QGraphicsWidgetItem::QGraphicsWidgetItem(QWidget *widget, QGraphicsWidgetItem *parent)
    : QGraphicsItem(parent, 0), embeddedWidget(0), lastActiveView(0)
{
    setFlag(ItemIgnoresTransformations);
    setWidget(widget);
}

QWidget *QGraphicsWidgetItem::widget() const
{
    return embeddedWidget;
}

void QGraphicsWidgetItem::setWidget(QWidget *widget)
{
    if (embeddedWidget && embeddedWidget->parent() == (QObject *)this) {
        embeddedWidget->removeEventFilter(this);
        embeddedWidget->hide();
        embeddedWidget->setParent(0);
    }
    embeddedWidget = widget;
    if (embeddedWidget) {
        embeddedWidget->installEventFilter(this);
        adjust();
    }
}

QRectF QGraphicsWidgetItem::boundingRect() const
{
    return QRectF(0, 0, size.width(), size.height());
}

void QGraphicsWidgetItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    adjust();
}

int QGraphicsWidgetItem::type() const
{
    return Type;
}

bool QGraphicsWidgetItem::eventFilter(QObject *watched, QEvent *event)
{
    // Adjust the item when it's reparented or resized.
    if (watched == embeddedWidget) {
        switch (event->type()) {
        case QEvent::Resize:
        case QEvent::ParentChange:
            adjust();
            break;
        default:
            break;
        }
    }

    // Adjust the item when the mouse enters or leaves a scene.
    if (watched == scene()) {
        switch (event->type()) {
        case QEvent::Enter:
        case QEvent::Leave:
            adjust();
            break;
        default:
            break;
        }        
    }

    return QObject::eventFilter(watched, event);
}

QVariant QGraphicsWidgetItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSceneChange) {
        // Update the scene event filter as the item moves to a new scene.
        if (QGraphicsScene *oldScene = scene())
            oldScene->removeEventFilter(this);
        if (QGraphicsScene *newScene = qVariantValue<QGraphicsScene *>(value))
            newScene->installEventFilter(this);
    }
    return QGraphicsItem::itemChange(change, value);
}

void QGraphicsWidgetItem::adjust()
{
    QGraphicsScene *scene = this->scene();
    if (!scene || !embeddedWidget)
        return;

    // Update the graphics item's geometry if the widget's geometry has
    // changed.
    if (embeddedWidget->size() != size) {
        prepareGeometryChange();
        size = embeddedWidget->size();
    }

    // Find the active view if there's no active view, use the last known
    // active view.
    QList<QGraphicsView *> views = scene->views();
    QGraphicsView *activeView = 0;
    if (views.size() == 1) {
        activeView = views.first();
    } else {
        foreach (QGraphicsView *view, scene->views()) {
            if (view->viewport()->underMouse()) {
                activeView = view;
                break;
            }
        }
    }
    if (!activeView)
        activeView = lastActiveView;
    if (!activeView)
        return;
    lastActiveView = activeView;

    // Check if the item is visible in the active view.
    QTransform itemTransform = deviceTransform(activeView->viewportTransform());
    bool visibleInActiveView = (itemTransform.mapRect(QRect(0, 0, size.width(), size.height())).intersects(activeView->viewport()->rect()));
    if (!visibleInActiveView)
        return;

    // Reparent the widget if necessary.
    if (embeddedWidget->parentWidget() != activeView->viewport())
        embeddedWidget->setParent(activeView->viewport());

    // Move the widget to its new viewport position.
    QPoint viewportPos = itemTransform.map(QPointF(0, 0)).toPoint();
    embeddedWidget->move(viewportPos);
    embeddedWidget->show();
}
