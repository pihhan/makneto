
#ifndef AVPARTICIPANT_CONTROL_H
#define AVPARTICIPANT_CONTROL_H
#include <QObject>
#include <QString>

#include "gstvideowatcher.h"
#include "audiowatcher.h"
#include "volumebar.h"
#include "gstvideowidget.h"
#include "qtjinglesession.h"

#include "knotification.h"

class MediaManager;
class User;
class MaknetoContact;

/** @brief Non-graphic class to control sessions for
    one participant.

    This intends to be main class for managing one multimedia session
    with one participant, connecting media objects to roster item and
    GUI work.
*/
class AVParticipantControl : public QObject,
    public GstVideoWatcher,
    public GstAudioWatcher
{
    Q_OBJECT
    public:

    AVParticipantControl(MediaManager *manager, const QString &jid);
    virtual ~AVParticipantControl();

    QString name() const;
    void setName(const QString &name);

    QtJingleSession *session() const;
    void setSession(QtJingleSession *session);

    virtual void prepareWindowId();
    virtual void videoResolutionChanged(const FrameSize &size);

    virtual void setWindowId(unsigned long id);
    virtual void handleExpose();

    virtual void updateMessage(GstMessage *msg);
    virtual void updateVolumes(int channels, double *rms, double *peak, double *decay);

    void setSelected(bool s);
    bool selected() const;

    User *mucUser() const;
    void setMucUser(User *user);

    MaknetoContact *contact() const;
    void setContact(MaknetoContact *contact);

    void raisePopup();

    public slots:
    void notificationAccept();
    void notificationAcceptAudioVideo();
    void notificationDecline();

    signals:
    void acceptedAudio();
    void acceptedAudioVideo();
    void declined();
    void expose();
    void prepareWindow();

    void volumeChanged(int);

    private:

    MediaManager    *m_manager;
    QString m_name;
    QString m_jid;
    QString m_nickname;
    User    *m_mucuser;
    MaknetoContact *m_contact;
    VolumeBar   *m_volumebar;
    GstVideoWidget  *m_video;
    QtJingleSession *m_jsession;
    KNotification   *m_notification;
    bool    m_selected;
};

#endif


