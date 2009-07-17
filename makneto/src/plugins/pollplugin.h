#ifndef POLLPLUGIN_H
#define POLLPLUGIN_H

#include "plugin.h"

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtGui/QPushButton>
class QGridLayout;
class QGraphicsProxyWidget;
class QLineEdit;
class QPushButton;
class QMenu;
class QPointF;

class PollMainButton : public QPushButton
{
  Q_OBJECT
  QPointF lastPos, startPos;
  public:
    PollMainButton(const QIcon &icon, const QString &text, QWidget *parent);
  protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
  signals:
    void moved(const QPointF &);
    void positionChanged(const QPointF &);
    // If the poll is dragged away, it will always emit clicked signal;
    // signal pushed will be emited only if no drag preceded
    void pushed();
};

class PollPlugin : public Plugin
{
  Q_OBJECT
  private:
    bool m_minimized;
    QWidget *w;
    QGridLayout *layout;
    QString m_pollName;
    QMap<int, QString> m_questionList;
    QList<QMap<int, bool> > m_answerList;
    QList<QString> m_respondents;
    QMap<int, QWidget *> m_answerWidgets;
    QLineEdit *m_respondent;
    QList<QWidget *> m_allWidgets;
    PollMainButton *m_button;
    QMenu *m_contextMenu;
    void init();
    void recreateWidget();
  public:
    PollPlugin(QStringList list);
    PollPlugin(const QDomNode &node);
    PollPlugin();
    ~PollPlugin();
    bool getQuestions();
    QDomElement svg();
    void parseSvg(const QDomNode &svg);
  public slots:
    void send();
    void buttonClicked();
    void contextMenu(const QPoint &pos);
    void moved(const QPointF &by);
    void positionChanged(const QPointF &newLocalPos);
    void remove();
};

#endif // POLLPLUGIN_H
