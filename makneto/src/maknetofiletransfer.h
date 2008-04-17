/*
 * maknetofiletransfer.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETOFILETRANSFER_H
#define MAKNETOFILETRANSFER_H

#include <QObject>

namespace XMPP 
{
	class FileTransfer;
}

using namespace XMPP;

/**
 * This is Makneto file transfer class - manages file transfer
 *
 * @short Makneto file transfer class
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 */

class MaknetoFileTransfer : public QObject
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	MaknetoFileTransfer(FileTransfer *ft);

	/**
	* Destructor
	*/
	virtual ~MaknetoFileTransfer();


signals:

public slots:

private:
	FileTransfer *m_ft;
};

#endif // MAKNETOFILETRANSFER_H