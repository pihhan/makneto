/*
 * addcontactdialog.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "addcontactdialog.h"

#include <KLocale>

AddContactDialog::AddContactDialog(QWidget *parent) : KDialog(parent)
{
	QWidget *widget = new QWidget( this );

	ui.setupUi(widget);
	setMainWidget( widget );

	setCaption(i18n("Add contact"));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setModal(true);
}

AddContactDialog::~AddContactDialog()
{
}

#include "addcontactdialog.moc"
