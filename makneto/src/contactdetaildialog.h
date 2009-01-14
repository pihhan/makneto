/*
 * contactdetaildialog.h
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@gmail.com>
 */
#ifndef CONTACTDETAILDIALOG_H
#define CONTACTDETAILDIALOG_H

#include <KDialog>
#include "ui_contact_detail.h"

class contactDetailDialog : public KDialog
{
  Q_OBJECT

public:
  contactDetailDialog(QWidget *parent, QString jid);
  ~contactDetailDialog(void);
protected Q_SLOTS:
  virtual void slotButtonClicked(int button);
  
private:
  Ui_ContactDetail ui;
  QString contactJID;
  void okClicked();
};


#endif