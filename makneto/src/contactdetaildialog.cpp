/*
 * contactdetaildialog.cpp
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@gmail.com>
 */

#include "contactdetaildialog.h"

contactDetailDialog::contactDetailDialog(QWidget *parent, QString jid) : KDialog(parent), contactJID(jid)
{
  QWidget *widget = new QWidget(this);
  
  ui.setupUi(widget);
  setMainWidget(widget);

  setCaption(i18n("Add contact"));
  setButtons(KDialog::Ok | KDialog::Cancel);
  setModal(true);
}

contactDetailDialog::~contactDetailDialog(void)
{
}

void contactDetailDialog::slotButtonClicked(int button)
{
  if (button == KDialog::Ok)
    okClicked();
  else
    KDialog::slotButtonClicked(button);
}

void contactDetailDialog::okClicked(void)
{
  accept();
}

#include "contactdetaildialog.moc"
