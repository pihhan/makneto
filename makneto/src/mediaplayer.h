/*
 * mediaplayer.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QtGui/QDialog>

#include <Phonon/MediaObject>
#include <Phonon/MediaSource>

class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class KToolBar;
class KAction;
class QBuffer;

namespace Phonon
{
	class VideoWidget;
	class MediaObject;
	class AudioOutput;
	class SeekSlider;
	class VolumeSlider;
}


class MediaPlayer: public QDialog
{
	Q_OBJECT
public:
	MediaPlayer(QWidget * parent = 0);

	void setCurrentSource(const Phonon::MediaSource &source);

public slots:
	void playbackStart();
	void playbackStop();
	void viewFullscreen();

private slots:
	void stateChanged(Phonon::State state);
	void hasVideoChanged(bool hasVideo);

private:
	void createButtons();

	QVBoxLayout *m_mainLayout;
	QLabel *m_label;
	Phonon::MediaObject *m_media;
	Phonon::VideoWidget *m_video;
	Phonon::AudioOutput *m_audio;
	Phonon::SeekSlider *m_seekSlider;
	Phonon::VolumeSlider *m_volumeSlider;
	KToolBar *m_playerToolBar;

	QBuffer *m_buffer;
	KAction *actionPlaybackStart;
	KAction *actionPlaybackStop;
	KAction *actionSeekForward;
	KAction *actionSeekBackward;
	KAction *actionViewFullscreen;
};

#endif //MEDIAPLAYER_H
