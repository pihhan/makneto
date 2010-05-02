
#ifndef VIDEO_CONFERENCE_WINDOW_H
#define VIDEO_CONFERENCE_WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

/** @brief Widget for main window of video conference. */
class VideoConferenceWindow : public QWidget
{
    public:
    VideoConferenceWindow(QWidget *parent = NULL);

    public Q_SLOTS:
    void initiateCall();

    private:
    // selfcontactwidget
    // remote contacts widget
    QPushButton *m_callBtn;
    QLineEdit   *m_targetInput;
    QWidget     *m_selfimage;

    
};

#endif

