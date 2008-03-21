/*
 * maknetoaccount.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef MAKNETOACCOUNT_H
#define MAKNETOACCOUNT_H

/**
 * This is Makneto account class
 *
 * @short Makneto account
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */


class MaknetoAccount
{
public:
	/**
	* Default constructor
	*/
	MaknetoAccount() { };

	/**
	* Destructor
	*/
	virtual ~MaknetoAccount() { };

	QString jid() { return m_jid; }
	QString password() { return m_password; }
	QString resource() { return m_resource; }
	QString host() { return m_host; }
private:
	QString m_jid;
	QString m_password;
	QString m_resource;
	QString m_host;
};

#endif // MAKNETOACCOUNT_H
