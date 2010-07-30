#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtCore/QObject>
#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>

class QGraphicsItem;
class QSize;

class Plugin : public QObject
{
  Q_OBJECT
  protected:
    QGraphicsItem *m_graphicsItem;
  public:
    Plugin(QGraphicsItem *graphicsItem = 0);

    /** Generates svg to transmit to the server */
    virtual QDomElement svg(QDomDocument &document) = 0;

    /** Parse incoming svg element */
    virtual void parseSvg(const QDomNode &svg) = 0;

    virtual void setGraphicsItem(QGraphicsItem *graphicsItem) { m_graphicsItem = graphicsItem; }
    QGraphicsItem *graphicsItem() { return m_graphicsItem; }
  signals:
    /** Emit this signal when you want to send data to the server */
    void sendChanges();
    void removed();
};

#endif // PLUGIN_H
