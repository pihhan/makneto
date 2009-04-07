#ifndef POLLPLUGIN_H
#define POLLPLUGIN_H

#include "plugin.h"

#include <QtCore/QStringList>
#include <QtCore/QMap>
class QGridLayout;
class QGraphicsProxyWidget;
class QLineEdit;
class QPushButton;

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
    QPushButton *m_button;
    void init();
    void recreateWidget();
  public:
    PollPlugin(QStringList list);
    PollPlugin(const QDomNode &node);
    PollPlugin();
    bool getQuestions();
    QDomElement svg();
    void parseSvg(const QDomNode &svg);
  public slots:
    void send();
    void buttonClicked();
  signals:
    void sendChanges();
};

#endif // POLLPLUGIN_H
