#ifndef POLLPLUGINCREATE_H
#define POLLPLUGINCREATE_H

#include <QtGui/QDialog>

class QLineEdit;
class QGridLayout;

class PollPluginCreate : public QDialog
{
  Q_OBJECT
  private:
    QList<QLineEdit *> questions;
    QPushButton *addButton;
    QGridLayout *layout;
    void addLine();
  public:
    PollPluginCreate();
    QMap<int, QString> getQuestions();
  private slots:
    void addQuestion();
};

#endif // POLLPLUGINCREATE_H
