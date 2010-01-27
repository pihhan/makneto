
#ifndef IRISBOT_H
#define IRISBOT_H

#include <QString>
#include <QObject>
#include <iris/xmpp_client.h>
#include <iris/xmpp.h>
#include <iris/xmpp_message.h>

#include "irisjinglemanager.h"

namespace  XMPP {

class Bot : public QObject
{
    Q_OBJECT

    public:
    Bot(QObject *parent = 0);

    void connectAs(const XMPP::Jid &user);
    QString prompt(const QString &text);
    void replyMessage(const XMPP::Message &m, const std::string &reply);

    public slots:
    void connected();
    void authenticated();
    void disconnected();
    void warning(int w);
    void needAuthParams(bool user, bool pwd, bool realm);
    void receiveMessage(const Message &m);
    void rosterFinished(bool success, int code, const QString &errmsg);
    void sessionStarted();
    void incomingXml(const QString &s);
    void outgoingXml(const QString &s);

    private:
    XMPP::Client    *m_client;
    XMPP::ClientStream *m_stream;
    XMPP::Connector *m_connector;
    XMPP::Jid       m_jid;

    IrisJingleManager *m_jm;
};

} // namespace

#endif

