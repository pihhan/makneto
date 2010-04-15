#include "wbforeign.h"

#include "plugins/plugin.h"
#include "plugins/pollplugin.h"
#include "wbwidget.h"

#include <QtGui/QGraphicsProxyWidget>
#include <QtCore/QDebug>

WbForeign::WbForeign(Plugin *plugin, QDomElement &svg, const QString &id, const qreal &index, const QString &parent, QGraphicsScene *scene)
  : WbItem(id)
{
  if (plugin)
    m_plugin = plugin;
  else
    m_plugin = 0;

  setParentWbItem(parent);
  setIndex(index);
  QDomNode node = svg.firstChildElement(), questions, question;
  if (!node.isNull())
  {
    if (node.nodeName().compare("poll") == 0)
    {
      _type = Poll;
      m_plugin = new PollPlugin(node);
    }
  }
  connect(m_plugin, SIGNAL(sendChanges()), this, SLOT(sendChanges()));
  connect(m_plugin, SIGNAL(removed()), this, SLOT(remove()));
  scene->addItem(m_plugin->graphicsItem());
  m_scene = scene;
}

QRectF WbForeign::boundingRect() const
{
  return m_plugin->graphicsItem()->boundingRect();
}

void WbForeign::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  //WbItem::paint(painter, option, widget);
}

QGraphicsItem* WbForeign::graphicsItem()
{
  if (m_plugin)
    return m_plugin->graphicsItem();
  else
    return 0;
}

QDomElement WbForeign::svg()
{
  if (m_plugin)
    return m_plugin->svg();
  else
    return QDomElement();
}

QList<QString> WbForeign::parseSvg(QDomElement &_svg, bool emitChanges)
{
  //QList<QString> changed = WbItem::parseSvg(_svg, emitChanges);
  if (m_plugin)
    m_plugin->parseSvg(_svg);
  return QList<QString>();
}

void WbForeign::sendChanges()
{
  QDomDocument doc = document();
  QDomElement el = doc.createElement("content");
  el.appendChild(svg());

  emit contentChanged(id(), el.childNodes(), QDomNodeList());
  //QDomElement _svg = svg();
  //parseSvg(_svg, true);
}

WbItem* WbForeign::clone()
{
  QDomElement _svg = svg();
  WbItem* cloned = new WbForeign(0, _svg, id(), index(), parentWbItem(), 0);
  cloned->undos = undos;
  return cloned;
}

void WbForeign::remove()
{
  static_cast<WbScene *> (m_scene)->removeElement(id(), true);
}

