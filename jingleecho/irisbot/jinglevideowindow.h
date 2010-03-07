
#ifndef JINGLEVIDEOWINDOW_H
#define JINGLEVIDEOWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QCloseEvent>

#include "mediaconfig.h"
#include "qtjinglesession.h"

/** @brief Class for video dialog window. */
class JingleVideoWindow : public QWidget 
{
    Q_OBJECT
    public:
    JingleVideoWindow();

    unsigned long remoteVideoWindowId();
    unsigned long localVideoWindowId();

    void updateMediaConfig(MediaConfig &config);
    void setSession(QtJingleSession *qjs);
    QtJingleSession *session();

    void setMessage(const QString &msg);

    public Q_SLOTS:
    void terminated(SessionReason reason);


    protected:
    virtual void closeEvent(QCloseEvent *event);

    private:

    QWidget *m_remoteVideo;
    QWidget *m_localVideo;
    QPushButton *m_mute_btn;
    QPushButton *m_hold_btn;
    QPushButton *m_terminate_btn;
    QLabel      *m_msglabel;
    QtJingleSession *m_session;
};

#endif

