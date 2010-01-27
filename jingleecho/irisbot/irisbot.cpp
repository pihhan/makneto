
#include <iostream>
#include <QApplication>
#include <QtCrypto>

#include "irisbot.h"

#include <xmpp/xmpp-im/xmpp_tasks.h>
#include <xmpp/xmpp-im/xmpp_status.h>

#include "logger/logger.h"

#include <gst/gst.h>

using namespace XMPP;

Bot::Bot(QObject *parent)
    : QObject(parent)
{
    m_client = new Client();
    m_connector = new AdvancedConnector();
    m_stream = new ClientStream(m_connector);

    m_jm = new IrisJingleManager(m_client->rootTask());
}

void Bot::connectAs(const Jid &user)
{

    connect(m_stream, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_stream, SIGNAL(authenticated()), this, SLOT(authenticated()));
    connect(m_stream, SIGNAL(needAuthParams(bool,bool,bool)), 
            this, SLOT(needAuthParams(bool,bool,bool)));
    connect(m_stream, SIGNAL(incomingXml(QString)),
            this, SLOT(incomingXml(QString)) );
    connect(m_stream, SIGNAL(outgoingXml(QString)),
            this, SLOT(outgoingXml(QString)) );
    connect(m_stream, SIGNAL(warning(int)), this, SLOT(warning(int)) );
    connect(m_client, SIGNAL(disconnected()), this, SLOT(disconnected()) );
    connect(m_client, SIGNAL(messageReceived(const Message&)),
            this, SLOT(receiveMessage(const Message&)) );

    m_stream->setUsername(user.node());

    m_client->connectToServer(m_stream, user, true);
}

void Bot::connected()
{
    std::cout << "Connected." << std::endl;
    Jid j = m_client->jid();
    m_client->start(j.domain(), j.node(), QString(), j.resource());
    std::cout << "Starting XMPP..." << std::endl;
}

void Bot::authenticated()
{
    std::cout << "Authenticated." << std::endl;
    JT_Session *session = new JT_Session(m_client->rootTask());
    connect(session, SIGNAL(finished()), this, SLOT(sessionStarted()) );
    session->go();
}

void Bot::needAuthParams(bool user, bool pwd, bool realm)
{
    if (user) {
        QString u = prompt("Zadej uzivatele");
        m_stream->setUsername(u);
    }
    if (pwd) {
        QString p = prompt("Zadej viditelne heslo");
        m_stream->setPassword(p);
    }
    if (realm) {
        QString r = prompt("Zadej realm");
        m_stream->setRealm(r);
    }
    m_stream->continueAfterParams();
}

QString Bot::prompt(const QString &text)
{
    std::cout << text.toStdString() << ": " << std::endl;
    std::string input;
    std::cin >> input;
    return QString::fromStdString(input);
}

void Bot::disconnected()
{
    std::cout << "disconnected." << std::endl;
    std::cout << m_stream->errorText().toStdString() << std::endl;
}

void Bot::warning(int w)
{
    std::cout << "warning: " << w << std::endl;
    m_stream->continueAfterWarning();
}

void Bot::receiveMessage(const Message &m)
{
    std::cout << "received message from " << m.from().full().toStdString()
        << ": " << m.body().toStdString() << std::endl;
   
    if (m.body() == "ping") {
        replyMessage(m, "pong!");
    } else if (!m.body().isEmpty()) {
        replyMessage(m, "I am iris bot, only thing i am able to do, is ping-pong.");
    }
}

void Bot::replyMessage(const Message &m, const std::string &reply)
{
    Message rm(m.from());
    rm.setThread(m.thread());
    rm.setType(m.type());
    rm.setBody(QString::fromStdString(reply));

    JT_Message *task = new JT_Message(
        m_client->rootTask(), rm);
    task->go();
}

void Bot::sessionStarted()
{
    connect(m_client, SIGNAL(rosterRequestFinished(bool,int,const QString&)),
            this, SLOT(rosterFinished(bool,int,const QString&)) );
    m_client->rosterRequest();
    std::cout << "Session started." << std::endl;
}

void Bot::rosterFinished(bool success, int code, const QString &errmsg)
{
    if (success) {
        std::cout << "Roster received." << std::endl;
        Status s(Status::Online, "irisbot here!");
        m_client->setPresence(s);
    } else {
        std::cout << "Roster retrieving failed, code " <<
            code << " reason: " << errmsg.toStdString() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    char *jid = NULL;
    if (argc < 2) {
        std::cerr << "Je treba zadat jid!" << std::endl;
        return 1;
    } else {
        jid = argv[1];
    }

    gst_init(&argc, &argv);

    QCA::Initializer qcainit;
    QApplication app(argc, argv);

    Bot bot;
    bot.connectAs(jid);

    app.exec();

    
    return 0;
}

void Bot::incomingXml(const QString &s)
{
    logit << "<<<" << s.toStdString() << std::endl;
}

void Bot::outgoingXml(const QString &s)
{
    logit << ">>>" << s.toStdString() << std::endl;
}


