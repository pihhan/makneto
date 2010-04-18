
#include <QBrush>
#include <QSizePolicy>
#include "chatoutput.h"

ChatOutput::ChatOutput(QWidget *parent) : QTextEdit(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::ClickFocus);
}

void ChatOutput::setNick(const QString &nick)
{
  m_nick = nick;
}

void ChatOutput::infoMessage(const QString &text)
{
  QTextCharFormat f = currentCharFormat();
  f.setFontItalic(true);
  f.setForeground(QBrush(Qt::green));
  setCurrentCharFormat(f);
  append(text);
//  append("<font color=\"#00FF00\"><i>" + text + "</i></font>");
}

void ChatOutput::myMessage(const QString &text)
{
  QTextCharFormat f = currentCharFormat();
  f.setFontItalic(false);
  f.setForeground(QBrush(Qt::blue));
  setCurrentCharFormat(f);
  append(text);
  
//  append("<font color=\"#0000FF\"><b>Me:</b></font> " + text);
}

void ChatOutput::errorMessage(const QString &text)
{
  QTextCharFormat f = currentCharFormat();
  f.setFontItalic(false);
  f.setForeground(QBrush(Qt::red));
  setCurrentCharFormat(f);
  append(QString(tr("Error: %1")).arg(text));

}

void ChatOutput::incomingMessage(const QString &text)
{
  QString newText(text);
  newText.replace(QRegExp("([^:]+:)(.*)"), "<font color=\"#0000FF\">\\1</font>\\2");
  if (!m_nick.isEmpty() && newText.section(':', 1, 1).contains(m_nick, Qt::CaseInsensitive))
    newText = "<b>" + newText + "</b>";
  append(newText);
}
