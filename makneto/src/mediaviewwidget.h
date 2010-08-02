
#ifndef MEDIATOOLBAR_H
#define MEDIATOOLBAR_H

#include <QWidget>
#include <QLayout>
#include <QSlider>
#include <QLabel>
#include <kaction.h>

#include "mediavideoselector.h"

class SessionView;

/** @brief Widget to display media controls and video output(s). */
class MediaViewWidget : public QWidget {
    Q_OBJECT
    public:

    MediaViewWidget(SessionView *parent=NULL);
    virtual ~MediaViewWidget();
    
    void setVideoSupported(bool is);
    void setAudioSupported(bool is);
    void configureToolbarForUser();
    void printText(const QString &txt);

    public slots:
    void toggleVideo();
    void toggleAudio();
    void mute();
    void hold();
    void fullscreen();
    void terminate();

    void modifyVolume(int volume);

    protected:

    void createToolbar();
    void createToolbarActions();

    private:
    KAction *toggleVideoAct;
    KAction *toggleAudioAct;
    KAction *muteAct;
    KAction *holdAct;
    KAction *terminateAct;
    KAction *fullscreenAct;


    QLayout *toolbarLayout;

    MediaVideoSelector *m_selector;
    SessionView *m_view;
    QSlider *m_volumeSlider;
    QLabel  *m_msglabel;
};

#endif

