
#ifndef AVPARTICIPANTWIDGET_H
#define AVPARTICIPANTWIDGET_H

#include <QWidget>
#include <QLabel>

#include "gstvideowatcher.h"
#include "audiowatcher.h"
#include "volumebar.h"
#include "gstvideowidget.h"
#include "qtjinglesession.h"

/** @brief Widget to control video and audio output
    from one remote participant.
*/
class AVParticipantWidget : public QWidget,
    public GstVideoWatcher,
    public GstAudioWatcher
{
    Q_OBJECT
    public:
    AVParticipantWidget(QWidget *parent = 0);

    QString name() const;
    void setName(const QString &name);

    QtJingleSession *session() const;
    void setSession(QtJingleSession *session);

    virtual void prepareWindowId();
    virtual void videoResolutionChanged(const FrameSize &size);

    virtual void setWindowId(unsigned long id);
    virtual void handleExpose();

    virtual void updateVolumes(int channels, double *rms, double *peak, double *decay);

    private:

    QLabel *m_name;
    VolumeBar   *m_volumebar;
    GstVideoWidget  *m_video;
    QtJingleSession *m_jsession;
};

#endif

