
#include "knotification.h"
#include "avoutput.h"
#include "avparticipantcontrol.h"
#include "maknetocontact.h"
#include "muccontrol.h"

AVParticipantControl::AVParticipantControl(MediaManager *manager, const QString &jid)
    : m_manager(manager), m_jid(jid), m_mucuser(0), 
      m_contact(0), m_selected(false)
{

}

AVParticipantControl::~AVParticipantControl()
{
    if (m_notification)
        delete m_notification;
}

QString AVParticipantControl::name() const
{
    return m_name;;
}

void AVParticipantControl::setName(const QString &name) 
{
    m_name = name;
}


void AVParticipantControl::prepareWindowId()
{
    // noop, window is now created with widget itself.
}

void AVParticipantControl::videoResolutionChanged(const FrameSize &size)
{
    m_video->setVideoSize(QSize(size.width, size.height));
}

void AVParticipantControl::setWindowId(unsigned long id)
{
    if (output())
        output()->setWindowId(id);
}

void AVParticipantControl::handleExpose()
{
    if (output())
        output()->expose();
}


QtJingleSession *AVParticipantControl::session() const
{
    return m_jsession;
}

void AVParticipantControl::setSession(QtJingleSession *session)
{
    m_jsession = (session);
}

void AVParticipantControl::updateMessage(GstMessage *msg)
{
    const char *srcname = GST_MESSAGE_SRC_NAME(msg);
    const char *type_name = GST_MESSAGE_TYPE_NAME(msg);

    qDebug() << "GstMessage from obj " << srcname << " of type " << type_name;

}

void AVParticipantControl::updateVolumes(int channels, double *rms, double *peak, double *decay)
{
}

void AVParticipantControl::setSelected(bool s)
{
    m_selected = s;
}

bool AVParticipantControl::selected() const
{
    return m_selected;
}

User *AVParticipantControl::mucUser() const
{
    return m_mucuser;
}

void AVParticipantControl::setMucUser(User *user)
{
    m_mucuser = user;
}


MaknetoContact *AVParticipantControl::contact() const
{
    return m_contact;
}

void AVParticipantControl::setContact(MaknetoContact *contact)
{
    m_contact = contact;
}

/** @brief Create and raise popup notification. */
void AVParticipantControl::raisePopup()
{
    KNotification *notify = new KNotification("incoming_call");
    QString nick = m_jid;
    bool audio = false;
    bool video = false;
    if (m_contact)
        nick = m_contact->name();
    if (m_jsession->session()->remoteContentsWithType(MEDIA_VIDEO) > 0) 
        video = true;
    if (m_jsession->session()->remoteContentsWithType(MEDIA_AUDIO) > 0)
        audio = true;

    QString calltype;
    if (audio && video)
        calltype = i18n("Audio/video call");
    else if (video)
        calltype = i18n("Video call");
    else if (audio)
        calltype = i18n("Audio call");
    else calltype = i18n("call without any media");
       
    notify->setText(i18n("%1 is requesting %2. Do you want to accept?",
        nick,calltype));
    notify->setActions(i18n("Accept,Decline").split(","));
    connect(notify, SIGNAL(action1Activated()), this, 
        SLOT(notificationAccept()));
    connect(notify, SIGNAL(action2Activated()), this, 
        SLOT(notificationDecline()));
    m_notification = notify;
    notify->sendEvent();

}

void AVParticipantControl::notificationAccept()
{
    emit acceptedAudio();
    m_notification->close();
}

void AVParticipantControl::notificationAcceptAudioVideo()
{
    emit acceptedAudioVideo();
    m_notification->close();
}

void AVParticipantControl::notificationDecline()
{
    emit declined();
    m_notification->close();
}

void AVParticipantControl::setVideo(bool enable)
{
    if (enable)
        enableVideo();
    else
        disableVideo();
}

void AVParticipantControl::enableVideo()
{
    AVOutput *avo = videoOutput();
    if (avo)
        avo->enableVideo();
}

void AVParticipantControl::disableVideo()
{
    AVOutput *avo = videoOutput();
    if (avo)
        avo->disableVideo();
}

void AVParticipantControl::setAudio(bool enable)
{
    if (enable)
        enableAudio();
    else
        disableAudio();
}

void AVParticipantControl::enableAudio()
{
}

void AVParticipantControl::disableAudio()
{
}

