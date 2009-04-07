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

#include <QtCore/QObject>
#include <QtGui/QGraphicsItem>

class QGraphicsView;

class QGraphicsWidgetItem : public QObject, public QGraphicsItem
{
public:
    QGraphicsWidgetItem(QGraphicsWidgetItem *parent = 0);
    QGraphicsWidgetItem(QWidget *widget, QGraphicsWidgetItem *parent = 0);

    QWidget *widget() const;
    void setWidget(QWidget *widget);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    enum { Type = UserType + 1 /* add your type here */ };
    int type() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    Q_DISABLE_COPY(QGraphicsWidgetItem)

    QWidget *embeddedWidget;
    QGraphicsView *lastActiveView;
    QSize size;
    void adjust();
};
