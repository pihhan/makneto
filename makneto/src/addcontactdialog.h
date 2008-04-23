/*
 * addcontactdialog.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <KDialog>
#include "ui_add_contact.h"

class AddContactDialog : public KDialog
{
	Q_OBJECT

public:
	AddContactDialog(QWidget *parent = 0);
	~AddContactDialog();

private:
	Ui::AddContact ui;
};

#endif //ADDCONTACTDIALOG_H
