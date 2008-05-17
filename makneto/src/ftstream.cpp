/*
 * ftstream.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "ftstream.h"

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QTimer>

FTStream::FTStream(QBuffer *buffer, qlonglong fileSize, QObject *parent) : m_fileSize(fileSize)
{
	qDebug("FTStream::FTStream()");

	m_buffer = buffer;
	m_sentBytes = 0;
	m_endOfDataSent = false;
	
	// this stream is not seekable
	setStreamSeekable(false);

	// need to fill otherwise Phonon doesn't call needData
	setStreamSize(fileSize);

	// and timer for pushing data to Phonon
	m_timer = new QTimer;
	connect(m_timer, SIGNAL(timeout()), SLOT(moreData()));
	m_timer->setInterval(0);
}

FTStream::~FTStream()
{
}

void FTStream::reset()
{
	qDebug("FTStream::reset()");

	m_buffer->reset();
/*	m_sentBytes = 0;
	m_endOfDataSent = false;*/
}

void FTStream::moreData()
{
//	qDebug("FTStream::moreData(): %ll", m_buffer->size());

	if (m_buffer)
	{
		QByteArray data = m_buffer->read(4096);

// 		m_sentBytes += data.size();

		writeData(data);
	}

/*	if (!m_endOfDataSent || (m_sentBytes == m_fileSize))
	{
		endOfData();
		m_endOfDataSent = true;

		qDebug("FTStream::moreData(): end of data");

		setStreamSeekable(true);
	}*/
}

void FTStream::needData()
{
	qDebug("FTStream::needData(): %x", m_buffer->size());

	// start pushing data
	m_timer->start();
	moreData();
}

void FTStream::enoughData()
{
	qDebug("FTStream::enoughData()");

	// enough data, stop pushing
	m_timer->stop();
}

// void FTStream::seekStream(qint64 offset)
// {
// 	if (m_endOfDataSent)
// 		m_buffer->seek(offset);
// }
