
#ifndef JINGLEVIDEOWINDOW_H
#define JINGLEVIDEOWINDOW_H

#include <QWidget>
#include <QPushButton>

#include "mediaconfig.h"

/** @brief Class for video dialog window. */
class JingleVideoWindow : public QWidget 
{
    Q_OBJECT
    public:
    JingleVideoWindow();

    unsigned long remoteVideoWindowId();
    unsigned long localVideoWindowId();

    void updateMediaConfig(MediaConfig &config);

    private:

    QWidget *m_remoteVideo;
    QWidget *m_localVideo;
    QPushButton *m_mute_btn;
    QPushButton *m_hold_btn;
    QPushButton *m_terminate_btn;
};

#endif

