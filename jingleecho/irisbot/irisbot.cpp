
#include <iostream>

#include <QApplication>
#include <QtCrypto>
#include <QSettings>

#include "irisbot.h"

#include <xmpp/xmpp-im/xmpp_tasks.h>
#include <xmpp/xmpp-im/xmpp_status.h>

#include "logger/logger.h"

#include <glib.h>
#include <gst/gst.h>

#include <QHostAddress>

using namespace XMPP;

QSettings *settings = NULL;

char * calljid = NULL;
char * configpath = NULL;
char * myjid = NULL;
char * password = NULL;
gboolean audiocall = false;
gboolean videocall = false;
gboolean avcall = false;

typedef enum {
    CALL_NONE,
    CALL_AUDIO,
    CALL_VIDEO,
    CALL_AUDIOVIDEO,
} CallType;

static GOptionEntry entries[] = 
{
    { "config", 'c', 0, G_OPTION_ARG_STRING, &configpath, "Path to config file", NULL },
    { "myjid", 'm', 0, G_OPTION_ARG_STRING, &myjid, "Full jid to use, if not specified by config file.", NULL },
    { "password", 'p', 0, G_OPTION_ARG_STRING, &password, "Password used to authenticate with.", NULL },
    { "target", 't', 0, G_OPTION_ARG_STRING, &calljid, "Full jid to call, after going online.", NULL },
    { "acall", 'a', 0, G_OPTION_ARG_NONE, &audiocall, "Initiate audio call after going online.", NULL },
    { "vcall", 'v', 0, G_OPTION_ARG_NONE, &videocall, "Initiate video call after going online.", NULL },
    { "avcall", 'A', 0, G_OPTION_ARG_NONE, &avcall, "Initiate audio and video call after going online.", NULL },
    { 0, 0, 0, G_OPTION_ARG_NONE, 0, 0, 0 }
};



Bot::Bot(QObject *parent)
    : QObject(parent)
{
    m_client = new Client(parent);
    m_connector = new AdvancedConnector(parent);
    m_stream = new ClientStream(m_connector, NULL, parent);

    m_jm = new IrisJingleManager(m_client->rootTask());

    m_configureStun = false;
    if (settings) {
        QString stunhost = settings->value("xmpp/stun").toString();

        if (!stunhost.isEmpty()) {
            stunResolve(stunhost);
            //m_configureStun = true;
            //connect(this, SIGNAL(stunConfigured()), this, SLOT(doCall()));
        }

        bool plainpass = settings->value("xmpp/plaintext").toBool();
        if (plainpass)
            m_stream->setAllowPlain(ClientStream::AllowPlain);
    }
}

Bot::~Bot()
{
    if (m_client)
        delete m_client;
}

void Bot::connectAs(const Jid &user)
{

    connect(m_stream, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_stream, SIGNAL(authenticated()), this, SLOT(authenticated()));
    connect(m_stream, SIGNAL(needAuthParams(bool,bool,bool)), 
            this, SLOT(needAuthParams(bool,bool,bool)));
    connect(m_client, SIGNAL(xmlIncoming(QString)),
            this, SLOT(incomingXml(QString)) );
    connect(m_client, SIGNAL(xmlOutgoing(QString)),
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
    Jid j = m_stream->jid();
    m_client->start(j.domain(), j.node(), QString(), j.resource());
    std::cout << "Starting XMPP..." << std::endl;
}

void Bot::authenticated()
{
    std::cout << "Authenticated." << std::endl;
    JT_Session *session = new JT_Session(m_client->rootTask());
    connect(session, SIGNAL(finished()), this, SLOT(sessionStarted()) );
    session->go(true);
}

void Bot::needAuthParams(bool user, bool pwd, bool realm)
{
    if (settings)
        settings->beginGroup("xmpp");
    if (user) {
        QString u;
        if (settings) {
            u = settings->value("username").toString();
            if (u.isEmpty()) {
                QString jidstr = settings->value("jid").toString();
                PJid jid(jidstr);
                u = jid.node();
            }
        }
        if (u.isEmpty() && myjid) {
            PJid jid(myjid);
            u = jid.node();
        }
        if (u.isEmpty()) {
            u = prompt("Zadej uzivatele");
        }
        m_stream->setUsername(u);
    }
    if (pwd) {
        QString p;
        if (settings) {
             p = settings->value("password").toString();
        }
        if (p.isEmpty()) 
            p = QString(password);
        if (p.isEmpty()) {
            p = prompt("Zadej viditelne heslo");
        }
        m_stream->setPassword(p);
    }
    if (realm) {
        QString r;
        if (settings) {
            r = settings->value("realm").toString();
        }
        if (r.isEmpty())
            QString r = prompt("Zadej realm");
        m_stream->setRealm(r);
    }
    m_stream->continueAfterParams();
    if (settings)
        settings->endGroup();
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

    //m_client->deleteLater();
    QCoreApplication::exit(0);
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
    } else if (m.body().startsWith("I am iris bot,")) {
        LOGGER(logit) << "Message loop detected. Not responding to message." << std::endl;
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

        if (!m_configureStun)
            doCall();
    } else {
        std::cout << "Roster retrieving failed, code " <<
            code << " reason: " << errmsg.toStdString() << std::endl;
    }
}

void Bot::doCall()
{
    if (calljid) {
        PJid callee(calljid);
        if (callee) {
            if (avcall || (audiocall && videocall)) {
                m_jm->initiateAudioVideoSession(callee, m_client->jid());
            } else if (audiocall) {
                m_jm->initiateAudioSession(callee, m_client->jid());
            } else if (videocall) {
                m_jm->initiateVideoSession(callee, m_client->jid());
            }
        } else {
            std::cerr << "K zavolani predano neplatne JID" 
                << calljid << std::endl;
        }
    }
}


void printHelp(const char *myname)
{
    std::cout << "Pouziti:" << std::endl
        << myname << " user@server/resource" << std::endl
        << myname << " -f config.ini" << std::endl;
}

int main(int argc, char *argv[])
{
    GOptionContext *context = NULL;
    GError *error = NULL;

    context = g_option_context_new("iris bot, jingle tester");
    g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        std::cerr << "Chypa parsovani parametru: " 
                << error->message << std::endl;
        std::cerr << g_option_context_get_help(context, true, 
            g_option_context_get_main_group(context))
                << std::endl;
        return 1;
    }

    if (configpath) {
        settings = new QSettings(configpath, QSettings::IniFormat);
        if (settings->status() != QSettings::NoError) {
            std::cerr << "Nacitani souboru " << configpath
                    << " selhalo:" << settings->status() << std::endl;
            return 1;
        } else {
            QString j = settings->value("xmpp/jid").toString();
            myjid = strdup( j.toLocal8Bit().data());
        }
    }

    if (!myjid) {
        std::cerr << "JID pripojeni nebylo zadano, ani v konfiguraci. koncim."
                  << std::endl;
        return 1;
    }

    gst_init(&argc, &argv);

    QCA::Initializer qcainit;
    QCoreApplication app(argc, argv);

    Bot *bot = new Bot();
    bot->connectAs(myjid);

    return app.exec();
}

void Bot::incomingXml(const QString &s)
{
    logit << "<<< " << s.toStdString() << std::endl;
}

void Bot::outgoingXml(const QString &s)
{
    logit << ">>> " << s.toStdString() << std::endl;
}

void Bot::stunResolve(const QString &hostname)
{
    QHostAddress address(hostname);
    if (address.protocol() != QAbstractSocket::UnknownNetworkLayerProtocol) {
        // it is valid address, ipv4 or ipv6, no resolver is needed
        m_jm->setStun(hostname.toStdString());
        emit stunConfigured();
    }
    int rid = 
    QHostInfo::lookupHost(hostname, this, SLOT(stunHostResolved(QHostInfo)));

}

/** @brief This slot uses resolved name from stunResolve, and configures Stun
    server to found IP, if query was successful. */
void Bot::stunHostResolved(const QHostInfo info)
{
    if (info.error() == QHostInfo::NoError) {
        QHostAddress a = info.addresses().first();
        std::string ip = a.toString().toStdString();
        m_jm->setStun(ip);
        emit stunConfigured();
        LOGGER(logit) << "Stun resolved: " << ip << std::endl;
    } else {
        std::cerr << "Resolver  pro " << info.hostName().toStdString() 
            << " vratil chybu: " 
            << info.errorString().toStdString() << std::endl;
    }
}

