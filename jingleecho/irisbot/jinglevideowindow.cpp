
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "jinglevideowindow.h"
#include "logger/logger.h"

JingleVideoWindow::JingleVideoWindow()
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

    QHBoxLayout *buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(m_mute_btn);
    buttonlayout->addWidget(m_hold_btn);
    buttonlayout->addWidget(m_terminate_btn);

    QVBoxLayout *formlayout = new QVBoxLayout();
    formlayout->addLayout(videolayout);
    formlayout->addLayout(buttonlayout);
   
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

