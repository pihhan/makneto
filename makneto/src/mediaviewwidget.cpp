
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KAction>
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QLabel>

#include "mediaviewwidget.h"
#include "sessionview.h"
#include "makneto.h"
#include "maknetocontactlist.h"
#include "maknetocontact.h"


MediaViewWidget::MediaViewWidget(SessionView *parent)
    : QWidget(parent), m_view(parent)
{
    QVBoxLayout *l = new QVBoxLayout();
    setLayout(l);

    m_msglabel = new QLabel("init label");
    l->addWidget(m_msglabel);
    
    m_selector = new MediaVideoSelector();
    l->addWidget(m_selector);

    createToolbarActions();
    createToolbar();

    l->addLayout(toolbarLayout);
}


MediaViewWidget::~MediaViewWidget()
{
}

/** @brief Create toolbar actions. */
void MediaViewWidget::createToolbarActions()
{
    toggleVideoAct = new KAction(KIcon("camera-web"), "toggleVideo", this);
    connect(toggleVideoAct, SIGNAL(triggered()), SLOT(toggleVideo()));

    toggleAudioAct = new KAction(KIcon("audio-input-microphone"), 
        "toggleAudio", this);
    connect(toggleAudioAct, SIGNAL(triggered()), SLOT(toggleAudio()) );

    muteAct = new KAction(KIcon("audio-volume-muted"), "mute", this);
    connect(muteAct, SIGNAL(triggered()), SLOT(mute()) );

    holdAct = new KAction(KIcon("hold"), "hold", this);
    connect(holdAct, SIGNAL(triggered()), SLOT(hold()) );

    terminateAct = new KAction(KIcon("terminate"), "terminate", this);
    connect(terminateAct, SIGNAL(triggered()), SLOT(terminate()) );
    
    fullscreenAct = new KAction(KIcon("fullscreen"), "fullscreen", this);
    connect(fullscreenAct, SIGNAL(triggered()), SLOT(fullscreen()) );
}

/** @brief Create toolbar layout, buttons, and stuff. */
void MediaViewWidget::createToolbar()
{
    QHBoxLayout *toolbarLayout = new QHBoxLayout();

    QToolButton *videoBtn = new QToolButton(this);
    videoBtn->setDefaultAction(toggleVideoAct);
    toolbarLayout->addWidget(videoBtn);

    QToolButton *audioBtn = new QToolButton(this);
    audioBtn->setDefaultAction(toggleAudioAct);
    toolbarLayout->addWidget(audioBtn);

    QToolButton *muteBtn = new QToolButton(this);
    muteBtn->setDefaultAction(muteAct);
    toolbarLayout->addWidget(muteBtn);

    QToolButton *holdBtn = new QToolButton(this);
    holdBtn->setDefaultAction(holdAct);
    toolbarLayout->addWidget(holdBtn);

    QToolButton *terminateBtn = new QToolButton(this);
    terminateBtn->setDefaultAction(terminateAct);
    toolbarLayout->addWidget(terminateBtn);

    QToolButton *fullscreenBtn = new QToolButton(this);
    fullscreenBtn->setDefaultAction(fullscreenAct);
    toolbarLayout->addWidget(fullscreenBtn);

    m_volumeSlider = new QSlider();
    connect(m_volumeSlider, SIGNAL(valueChanged(int)), SLOT(volumeChanged(int)) );

    this->toolbarLayout = toolbarLayout;
}


void MediaViewWidget::toggleVideo()
{
}

void MediaViewWidget::toggleAudio()
{
}

void MediaViewWidget::mute()
{
}

void MediaViewWidget::hold()
{
}

void MediaViewWidget::fullscreen()
{
}

void MediaViewWidget::terminate()
{
}

/** @brief Modify volume of selected participant. */
void MediaViewWidget::modifyVolume(int volume)
{
    double rvolume = volume / 100.0;
}

void MediaViewWidget::setVideoSupported(bool is)
{
    toggleVideoAct->setEnabled(is);
    fullscreenAct->setEnabled(is);
}

void MediaViewWidget::setAudioSupported(bool is)
{
    toggleAudioAct->setEnabled(is);
    muteAct->setEnabled(is);
    holdAct->setEnabled(is);
    m_volumeSlider->setEnabled(is);
}

void MediaViewWidget::configureToolbarForUser()
{
    QString jid = m_view->jid();
    Makneto *makneto = m_view->makneto();
    MaknetoContactList *cl = makneto->getContactList();
    MaknetoContact *contact = cl->getContact(jid);

    if (contact) {
        setVideoSupported(contact->supportsVideo());
        setAudioSupported(contact->supportsAudio()); 
    } else {
        printText(i18n("This contact was not found inside roster."));
    }
}

void MediaViewWidget::printText(const QString &txt)
{
    m_msglabel->setText(txt);
}

