/*
 * addcontactdialog.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "addcontactdialog.h"

#include "xmpp_jid.h"

#include <KLocale>
#include <KMessageBox>
#include <QtGui/QLineEdit>

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

void AddContactDialog::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
		okClicked();
	else
		KDialog::slotButtonClicked(button);
}

void AddContactDialog::okClicked()
{
	if (ui.jabberID->text().isEmpty())
	{
		KMessageBox::error(this, i18n("You have to set correct Jabber ID!"), i18n("Error"));
		return;
	}

	if (!XMPP::Jid(ui.jabberID->text().simplified()).isValid())
	{
		KMessageBox::error(this, i18n("Incorrect Jabber ID! Please try again to set correct Jabber ID."), i18n("Error"));
		return;
	}

	addUser(XMPP::Jid(ui.jabberID->text().simplified()), "", ui.requestAuth->isChecked());

	accept();
}

#include "addcontactdialog.moc"
