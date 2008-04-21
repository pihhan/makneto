/*
 * ftstream.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef FTSTREAM_H
#define FTSTREAM_H

#include <Phonon/AbstractStream>

namespace Phonon {

class FTStream: public Phonon::AbstractStream
{
	Q_OBJECT

public:
	FTStream(const QBuffer &buffer, QObject *parent = 0);
	~FTStream();

protected:
	void needData();
	void enoughData();

private:
	QBuffer *m_buffer;

};

} // namespace Phonon
#endif // FTSTREAM_H
