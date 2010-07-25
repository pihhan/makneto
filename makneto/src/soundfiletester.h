
#ifndef SOUND_FILE_TESTER_H
#define SOUND_FILE_TESTER_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include "mediamanager.h"

/** \brief Class to test gst media player. */
class SoundFileTester : public QWidget
{
    Q_OBJECT
    public:
    SoundFileTester(MediaManager *mgr, QWidget *parent = 0);


    public slots:
    void find();
    void play();
    void stop();
	void pause();
	void displayPipeline();

    void reportMsg(const QString &msg);

    void playerStarted();
    void playerStopped();

    private:
    MediaManager *m_manager;
    QLineEdit   *edt_filename;
    QPushButton *btn_play;
    QPushButton *btn_stop;
	QPushButton *btn_pause;
    QPushButton *btn_find;
	QPushButton *btn_display;
    QLabel      *lbl_status;
};

#endif

