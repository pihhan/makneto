#include "pollplugin.h"
#include "qgraphicswidgetitem.h"
#include "pollplugincreate.h"

#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <kde4/kicon.h>

PollPlugin::PollPlugin(QStringList list) : Plugin()
{
  init();
  for (int i = 0; i < list.size(); i++)
  {
    m_questionList.insert(i, list[i]);
  }
  recreateWidget();
}

PollPlugin::PollPlugin(const QDomNode &node) : Plugin()
{
  init();
  parseSvg(node);
}

PollPlugin::PollPlugin() : Plugin()
{
  init();
}

void PollPlugin::init()
{
  w = new QWidget(0);
  layout = new QGridLayout(w);
  m_graphicsItem = new QGraphicsWidgetItem();
  m_minimized = false;
}

bool PollPlugin::getQuestions()
{
  bool res = false;
  PollPluginCreate *create = new PollPluginCreate();
  if (create->exec() == QDialog::Accepted)
  {
    m_questionList = create->getQuestions();
    recreateWidget();
    res = true;
  }
  delete create;
  return res;
}

void PollPlugin::recreateWidget()
{
  w->setUpdatesEnabled(false);
  dynamic_cast<QGraphicsWidgetItem *> (m_graphicsItem)->setWidget(0);
  // Delete all widgets
  while (!m_allWidgets.empty())
  {
    delete m_allWidgets.takeLast();
  }
  delete layout;
  delete w;
  m_answerWidgets.clear();

  // Recreate widget
  w = new QWidget(0);
  layout = new QGridLayout();
  // Create top button
  m_button = new QPushButton(KIcon("maknetopoll"), QString(), w);
  connect(m_button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
  layout->addWidget(m_button);
  // Don't create widget when minimized
  if (!m_minimized)
  {
    QLabel *label = new QLabel(m_pollName);
    layout->addWidget(label, 0, 1);

    int i = 0, j;
    QCheckBox *checkBox;
    // Mapping question ID's to indexes
    QList<int> indexes;
    foreach (int index, m_questionList.keys())
    {
      label = new QLabel(m_questionList.value(index, ""));
      indexes.append(index);
      m_allWidgets.append(label);
      layout->addWidget(label, 0, i + 2);
      checkBox = new QCheckBox();
      m_allWidgets.append(checkBox);
      layout->addWidget(checkBox, m_answerList.size() + 1, i + 2);
      m_answerWidgets.insert(index, checkBox);
      i++;
    }
    for (i = 0; i < m_answerList.size(); i++)
    {
      foreach (int index, m_answerList[i].keys())
      {
        j = indexes.indexOf(index);
        if (j < 0)
          continue;
        label = new QLabel();
        m_allWidgets.append(label);
        if (!m_answerList[i].contains(index))
          label->setText("?");
        else if (m_answerList[i].value(index))
          label->setText("YES");
        else
          label->setText("NO");
        layout->addWidget(label, i + 1, j + 2);
      }
    }
    for (i = 0; i < m_respondents.size(); i++)
    {
      label = new QLabel(m_respondents[i]);
      m_allWidgets.append(label);
      layout->addWidget(label, i + 1, 0);
    }
    m_respondent = new QLineEdit();
    m_allWidgets.append(m_respondent);
    layout->addWidget(m_respondent, m_answerList.size() + 1, 0, 1, 2);
    QPushButton *send = new QPushButton("Send");
    m_allWidgets.append(send);
    layout->addWidget(send, m_answerList.size() + 1, m_questionList.size() + 2);
    connect(send, SIGNAL(clicked(bool)), SLOT(send()));
  }
  w->setLayout(layout);
  w->setUpdatesEnabled(true);
  if (m_graphicsItem)
  {
    dynamic_cast<QGraphicsWidgetItem *> (m_graphicsItem)->setWidget(w);
  }
}

void PollPlugin::send()
{
  if (m_respondent->text().isEmpty())
  {
    m_respondent->setFocus();
    return;
  }

  QMap<int, bool> l;
  foreach (int i, m_answerWidgets.keys())
    l.insert(i, dynamic_cast<QCheckBox *> (m_answerWidgets.value(i))->checkState() == Qt::Checked);
  QString r = m_respondent->text();
  int index = m_respondents.indexOf(r);
  if (index < 0)
  {
    m_answerList.append(l);
    m_respondents.append(r);
  }
  else
  {
    m_answerList.replace(index, l);
    m_respondents.replace(index, r);
  }
  emit sendChanges();
  recreateWidget();
}

void PollPlugin::buttonClicked()
{
  m_minimized = !m_minimized;
  recreateWidget();
}

QDomElement PollPlugin::svg()
{
  QDomDocument doc;
  QDomElement svg, xml, el, child;
  QDomText text;
  svg = doc.createElement("foreignObject");
  svg.setAttribute("requiredExtensions", "http://makneto.org/plugins/poll");
  xml = doc.createElement("poll");
  xml.setAttribute("name", m_pollName);
  svg.appendChild(xml);
  el = doc.createElement("questions");
  xml.appendChild(el);
  foreach (int i, m_questionList.keys())
  {
    child = doc.createElement("question");
    child.setAttribute("id", i);
    el.appendChild(child);
    text = doc.createTextNode(m_questionList.value(i));
    child.appendChild(text);
  }
  if (m_answerList.size() > 0)
  {
    QDomElement answer;
    el = doc.createElement("answerList");
    xml.appendChild(el);
    qDebug() << "m_answerList.size(): " << m_answerList.size();
    for (int i = 0; i < m_answerList.size(); i++)
    {
      child = doc.createElement("answers");
      child.setAttribute("by", m_respondents[i]);
      el.appendChild(child);
      foreach (int j, m_answerList[i].keys())
      {
        answer = doc.createElement("answer");
        answer.setAttribute("id", j);
        child.appendChild(answer);
        text = doc.createTextNode((m_answerList[i].value(j)) ? "1" : "0");
        answer.appendChild(text);
      }
    }
  }
  return svg;
}

void PollPlugin::parseSvg(const QDomNode &svg)
{
  QDomElement root;
  if (svg.nodeName().compare("foreignObject") == 0)
    root = svg.firstChildElement().toElement();
  else
    root = svg.toElement();

  if (root.isNull())
    return;

  // Get name of the poll
  if (root.isElement())
  {
    m_pollName = root.toElement().attribute("name", "");
  }
  QDomElement questions = root.firstChildElement("questions"), question;
  if (!questions.isNull())
  {
    question = questions.firstChildElement("question");
    while (!question.isNull())
    {
      QString q;
      int id = question.attribute("id", "-1").toInt();
      // Question has to have an ID
      if (id != -1)
      {
        // Try to get text of the element
        if (question.firstChild().isText())
        {
          QString q = question.firstChild().toText().nodeValue();

          // Find question with the same ID
          if (m_questionList.contains(id))
          {
            // Owerwrite question
            m_questionList[id] = q;
          }
          else
          {
            // Append question
            m_questionList.insert(id, q);
          }
        }
      }
      // Continue with next question
      question = question.nextSiblingElement("question");
    }
  }
  QDomElement answerList = root.firstChildElement("answerList"), answersNode, answerNode;
  QString r;
  int index = -1, id;
  if (!answerList.isNull())
  {
    answersNode = answerList.firstChildElement("answers");
    // Iteration through all asnwers
    while (!answersNode.isNull())
    {
      // Obtain respondent's name
      r = answersNode.attribute("by", QString());
      // Exist this respondent?
      index = m_respondents.indexOf(r);
      if (index < 0)
        // If not, add him
        m_respondents.append(r);

      // Fill the answer list
      QMap<int, bool> answer;
      answerNode = answersNode.firstChildElement("answer");
      while (!answerNode.isNull())
      {
        // If answer has an ID, add the value, skip it otherwise
        id = answerNode.attribute("id", "-1").toInt();
        if (id != -1)
          answer.insert(id, answerNode.firstChild().toText().nodeValue() == "0" ? false : true);

        answerNode = answerNode.nextSiblingElement("answer");
      }
      // Insert or update answer list
      if (index < 0)
        m_answerList.append(answer);
      else
        m_answerList.replace(index, answer);

      answersNode = answersNode.nextSiblingElement("answers");
    }
  }
  recreateWidget();
}
