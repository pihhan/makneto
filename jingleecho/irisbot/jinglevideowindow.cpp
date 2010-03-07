
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "jinglevideowindow.h"
#include "logger/logger.h"

JingleVideoWindow::JingleVideoWindow()
    : m_session(0)
{
    m_localVideo = new QWidget();
    m_localVideo->resize(320,240);
    m_localVideo->setMinimumSize(160,120);
    m_remoteVideo = new QWidget();
    m_remoteVideo->resize(320,240);
    m_remoteVideo->setMinimumSize(160,120);

    QHBoxLayout *videolayout = new QHBoxLayout();
    videolayout->addWidget(m_remoteVideo);
    videolayout->addSpacing(10);
    videolayout->addWidget(m_localVideo);


    m_mute_btn = new QPushButton(tr("Mute"));
    m_hold_btn = new QPushButton(tr("Hold"));
    m_terminate_btn = new QPushButton(tr("Terminate"));

    m_msglabel = new QLabel();
    m_msglabel->setMaximumHeight(100);

    QHBoxLayout *buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(m_mute_btn);
    buttonlayout->addWidget(m_hold_btn);
    buttonlayout->addWidget(m_terminate_btn);

    QVBoxLayout *formlayout = new QVBoxLayout();
    formlayout->addLayout(videolayout);
    formlayout->addLayout(buttonlayout);
    formlayout->addWidget(m_msglabel);
   
    setLayout(formlayout); 
}

unsigned long JingleVideoWindow::remoteVideoWindowId()
{
    return m_remoteVideo->winId();
}

unsigned long JingleVideoWindow::localVideoWindowId()
{
    return m_localVideo->winId();
}

/** @brief Add parameters to video output with window id's for video Output. */
void JingleVideoWindow::updateMediaConfig(MediaConfig &config)
{
    MediaDevice ro = config.videoOutput();
    ro.addParameter(PayloadParameter("xwindow-id", remoteVideoWindowId()));
    config.setVideoOutput(ro);

    MediaDevice lo = config.localVideoOutput();
    lo.addParameter(PayloadParameter("xwindow-id", localVideoWindowId()));
    config.setLocalVideoOutput(lo);

    LOGGER(logit) << "Updated video output configuration to remote id: " 
        << remoteVideoWindowId() << " local id: " 
        << localVideoWindowId() << std::endl;   

    QApplication::syncX();
}

void JingleVideoWindow::setSession(QtJingleSession *qjs)
{
    m_session = qjs;

    connect(m_terminate_btn, SIGNAL(clicked()), m_session, SLOT(terminate()));
    connect(m_hold_btn, SIGNAL(clicked()), m_session, SLOT(hold()));
    connect(m_mute_btn, SIGNAL(clicked()), m_session, SLOT(mute()));
    connect(m_session, SIGNAL(terminated(SessionReason)), this, SLOT(terminated(SessionReason)));
}

QtJingleSession * JingleVideoWindow::session()
{
    return m_session;
}

void JingleVideoWindow::setMessage(const QString &msg)
{
    m_msglabel->setText(msg);
}

void JingleVideoWindow::closeEvent(QCloseEvent *event)
{
    if (m_session && m_session->session()->state() != JSTATE_TERMINATED) {
        event->accept();
    } else {
        event->ignore();
    }
}

void JingleVideoWindow::terminated(SessionReason reason)
{
    setMessage("Terminated: ");
}

