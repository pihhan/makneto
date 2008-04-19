/*
 * mediaplayer.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "mediaplayer.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QActionGroup>
#include <QBuffer>
#include <QDebug>


#include "kaction.h"
#include "ktoolbar.h"
#include "kiconloader.h"
#include "klocale.h"

#include <Phonon/BackendCapabilities>
#include <Phonon/ObjectDescription>
#include <Phonon/VideoWidget>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>

#include <iostream>

MediaPlayer::MediaPlayer(QWidget * parent)
{
	m_mainLayout = new QVBoxLayout;

	m_label = new QLabel("Unknow video from unknown contact");
	m_mainLayout->addWidget(m_label);

	m_media = new Phonon::MediaObject;
	connect(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(stateChanged(Phonon::State)));
	connect(m_media, SIGNAL(hasVideoChanged(bool)), this, SLOT(hasVideoChanged(bool)));


	// create video widget
	m_video = new Phonon::VideoWidget;
	m_mainLayout->addWidget(m_video, 1);

	// create audio device
	m_audio = new Phonon::AudioOutput(Phonon::VideoCategory);

	// create seek slider
	m_seekSlider = new Phonon::SeekSlider;
	m_seekSlider->setMediaObject(m_media);
	m_mainLayout->addWidget(m_seekSlider);

	// and toolbar
	m_playerToolBar = new KToolBar(this);
	m_playerToolBar->setIconDimensions(32);
	m_playerToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

	QActionGroup *groupMode = new QActionGroup(this);

	actionSeekBackward = new KAction(KIcon("media-seek-backward"), i18n("Seek backward"), groupMode);
	m_playerToolBar->addAction(actionSeekBackward);

	actionPlaybackStart = new KAction(KIcon("media-playback-start"), i18n("Play"), groupMode);
	m_playerToolBar->addAction(actionPlaybackStart);
	connect(actionPlaybackStart, SIGNAL(triggered()), this, SLOT(playbackStart()));
 
	actionPlaybackStop = new KAction(KIcon("media-playback-stop"), i18n("Stop"), groupMode);
	m_playerToolBar->addAction(actionPlaybackStop);
	connect(actionPlaybackStop, SIGNAL(triggered()), this, SLOT(playbackStop()));
	actionPlaybackStop->setEnabled(false);

	actionSeekForward = new KAction(KIcon("media-seek-forward"), i18n("Seek forward"), groupMode);
	m_playerToolBar->addAction(actionSeekForward);

	actionViewFullscreen = new KAction(KIcon("view-fullscreen"), i18n("View fullscreen"), groupMode);
	//m_playerToolBar->addAction(actionViewFullscreen);
	connect(actionViewFullscreen, SIGNAL(triggered()), this, SLOT(viewFullscreen()));

	m_volumeSlider = new Phonon::VolumeSlider;
	m_volumeSlider->setAudioOutput(m_audio);
	m_playerToolBar->addWidget(m_volumeSlider);
	
	m_mainLayout->addWidget(m_playerToolBar);

	//TODO: test

	// now lets create paths
	Phonon::createPath(m_media, m_video);
	Phonon::createPath(m_media, m_audio);

	QList<Phonon::AudioOutputDevice> caps = Phonon::BackendCapabilities::availableAudioOutputDevices();

	for(int i = 0; i<caps.size(); i++)
		qDebug(caps[i].name());

	setLayout(m_mainLayout);
}

void MediaPlayer::setCurrentSource(const Phonon::MediaSource &source)
{
	m_media->setCurrentSource(source);

	if (m_media->hasVideo())
	{
		m_video->show();
		resize(300, 350);
	}
	else
		m_video->hide();
}

void MediaPlayer::createButtons()
{
}

void MediaPlayer::playbackStart()
{
	std::cout << "MediaPlayer::playbackStart() = " << m_media->state() << std::endl;
	if (!m_media)
		return;

	switch (m_media->state())
	{
	case Phonon::StoppedState:
	case Phonon::PausedState:
		m_media->play();
		break;

	case Phonon::PlayingState:
		m_media->pause();
		break;

	default:
		break;
	}
}

void MediaPlayer::playbackStop()
{
	if (!m_media)
		return;

	m_media->stop();
	
}

void MediaPlayer::viewFullscreen()
{
	if (!m_video)
		return;

	//if (!m_video->fullScreen)
		m_video->setFullScreen(true);
	//else
	//	m_video->setFullScreen(false);	
}

void MediaPlayer::stateChanged(Phonon::State state)
{
	switch (state)
	{
	case Phonon::StoppedState:
		actionPlaybackStop->setEnabled(false);
	case Phonon::PausedState:
		actionPlaybackStart->setIcon(KIcon("media-playback-start"));
		break;

	case Phonon::PlayingState:
		actionPlaybackStart->setIcon(KIcon("media-playback-pause"));
		actionPlaybackStop->setEnabled(true);
		break;

	default:
		break;
	}
}

void MediaPlayer::hasVideoChanged(bool hasVideo)
{
	std::cout << "MediaPlayer::hasVideoChanged() = " << m_media->hasVideo() << std::endl;

	if (m_media->hasVideo())
		m_video->show();
	else
		m_video->hide();
}

#include "mediaplayer.moc"
