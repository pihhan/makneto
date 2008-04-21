/*
 * ftstream.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef FTSTREAM_H
#define FTSTREAM_H

#include <QtCore/QBuffer>

class QTimer;

#include <Phonon/AbstractMediaStream>

/**
 * This is Phonon media stream without seeking support,
 * which can read FileTransfer stream.
 *
 *
 * @short FTStream is media source for Phonon
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 */

class FTStream: public Phonon::AbstractMediaStream
{
	Q_OBJECT

public:
	FTStream(QBuffer *buffer, qlonglong fileSize, QObject *parent = 0);
	~FTStream();

protected:
	void reset();
	void needData();
	void enoughData();

private slots:
	void moreData();

private:
	QBuffer *m_buffer;
	QTimer *m_timer;

	bool m_reading;
	bool m_endOfDataSent;
};

#endif // FTSTREAM_H
