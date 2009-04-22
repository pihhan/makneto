#ifndef WBFOREIGN_H
#define WBFOREIGN_H

#include "wbitems.h"

class Plugin;

class WbForeign : public WbItem
{
  Q_OBJECT;
  enum Types { Poll, Other };
  private:
    QDomElement svgElement_;
    Types _type;
    QGraphicsItem *m_graphicsItem;
    Plugin *m_plugin;
    QGraphicsScene *m_scene;
  public:
    WbForeign(Plugin *plugin, QDomElement &svg, const QString &id, const qreal &index, const QString &parent = "root", QGraphicsScene *scene = 0);

    /*! \brief Returns the type of the item (0x87654998).*/
    int type() const { return 87654998; }

    /*! \brief Returns the stored SVG element.*/
    QDomElement svg();

    /*! \brief Saves the SVG element.*/
    QList<QString> parseSvg(QDomElement &, bool emitChanges = false);

    /*! \brief Returns a deep copy of this object but without a scene association.*/
    WbItem* clone();

    QGraphicsItem* graphicsItem();
  protected:
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

  public slots:
    void sendChanges();
    void remove();
};

#endif // WBFOREIGN_H
