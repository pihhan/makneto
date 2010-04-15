#ifndef CHATOUTPUT_H
#define CHATOUTPUT_H

#include <QtGui/QTextEdit>

class ChatOutput : public QTextEdit
{
  QString m_nick;
  public:
    ChatOutput(QWidget *parent);
    void setNick(const QString &nick);
    void infoMessage(const QString &text);
    void myMessage(const QString &text);
    void errorMessage(const QString &text);
    void incomingMessage(const QString &text);
};

#endif // CHATOUTPUT_H
