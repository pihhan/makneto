#include "pollplugincreate.h"

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QDialogButtonBox>
#include <QtCore/QMap>

PollPluginCreate::PollPluginCreate()
{
  QVBoxLayout *topLayout = new QVBoxLayout(this);
  layout = new QGridLayout(0);
  topLayout->addLayout(layout);

  addLine();

  addButton = new QPushButton(tr("Add question"), this);
  layout->addWidget(addButton, 0, 2);
  connect(addButton, SIGNAL(pressed()), this, SLOT(addQuestion()));

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  topLayout->addWidget(buttonBox);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QMap<int, QString> PollPluginCreate::getQuestions()
{
  QMap<int, QString> l;
  for (int i = 0; i < questions.size(); i++)
  {
    l.insert(i, questions[i]->text());
  }
  return l;
}

void PollPluginCreate::addLine()
{
  int row = questions.size();
  QLabel *label = new QLabel(QString(tr("Question #%1")).arg(row + 1), this);
  layout->addWidget(label, row, 0);
  QLineEdit *question = new QLineEdit(this);
  layout->addWidget(question, row, 1);
  label->setBuddy(question);
  questions << question;
}

void PollPluginCreate::addQuestion()
{
  layout->removeWidget(addButton);
  addLine();
  layout->addWidget(addButton, questions.size() - 1, 2);
}

