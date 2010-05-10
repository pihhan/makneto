/*
 * contactdetaildialog.h
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@gmail.com>
 * Copyleft 2009 Petr Mensik <pihhan@seznam.cz>
 */
#ifndef CONTACTDETAILDIALOG_H
#define CONTACTDETAILDIALOG_H

#include <KDialog>
#include <QByteArray>
#include "ui_contact_detail.h"
#include "maknetocontact.h"

class contactDetailDialog : public KDialog
{
  Q_OBJECT

public:
  contactDetailDialog(QWidget *parent, QString jid);
  ~contactDetailDialog(void);

  void setDetailText(const QString &text);
  void updateDetailText();
  void describeContact(MaknetoContact *contact);
  void describeFeatures(MaknetoContact *contact);
  void updatePhoto(const QByteArray &img);
protected Q_SLOTS:
  virtual void slotButtonClicked(int button);
  void detailsArrived();
  
private:
  Ui_ContactDetail ui;
  QString contactJID;
  QString featureDescription;
  QString contactDescription;
  void okClicked();
};


#endif

