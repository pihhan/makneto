/*
 * addcontactdialog.h
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <KDialog>
#include "ui_add_contact.h"

#include "xmpp_jid.h"

namespace XMPP
{
	class Jid;
}

class AddContactDialog : public KDialog
{
	Q_OBJECT

public:
	AddContactDialog(QWidget *parent = 0);
	~AddContactDialog();

signals:
	void addUser(const XMPP::Jid &, const QString &, bool);

protected Q_SLOTS:
	virtual void slotButtonClicked(int button);

private:
	Ui::AddContact ui;

	void okClicked();
};

#endif //ADDCONTACTDIALOG_H
