
#ifndef AVPARTICIPANTWIDGET_H
#define AVPARTICIPANTWIDGET_H

#include <QFrame>
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
class AVParticipantWidget : public QFrame,
    public GstVideoWatcher,
    public GstAudioWatcher
{
    Q_OBJECT
    public:
    AVParticipantWidget(QWidget *parent = 0);
    virtual ~AVParticipantWidget();

    QString name() const;
    void setName(const QString &name);

    QtJingleSession *session() const;
    void setSession(QtJingleSession *session);

    virtual void videoResolutionChanged(const FrameSize &size);

    virtual void setWindowId(unsigned long id);
    virtual void handleExpose();

    virtual void updateMessage(GstMessage *msg);
    virtual void updateVolumes(int channels, double *rms, double *peak, double *decay);

    void setSelected(bool s);
    bool selected() const;

    public slots:
    virtual void prepareWindowId();
    void videoExposed();
    void videoResolutionChanged(const QSize &size);

    void displayVolume(int volume);
    void setVolume(int volume);
    void setVolume(double volume);

    signals:
    void videoWindowIdChanged(WId id);

    private:

    QLabel *m_name;
    VolumeBar   *m_volumebar;
    GstVideoWidget  *m_video;
    QtJingleSession *m_jsession;
    bool    m_selected;
};

#endif

