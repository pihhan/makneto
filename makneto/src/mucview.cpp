/*
 * mucview.cpp
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@centrum.cz>
 */

#include "mucview.h"

#include <iostream>
#include <QInputDialog>
#include <QMessageBox>

MUCView::MUCView(QWidget *parent, Makneto *makneto) : m_makneto(makneto)
{
  m_mainlayout = new QVBoxLayout(this);
  m_buttonslayout = new QHBoxLayout(this);

  // buttons
  m_createMUC = new QPushButton(KIconLoader::global()->loadIcon("list-add-user", KIconLoader::Toolbar, KIconLoader:: SizeSmall), i18n("&Create MUC"), this);
  m_buttonslayout->addWidget(m_createMUC);
  connect(m_createMUC, SIGNAL(clicked(bool)), SLOT(createMUCClicked(bool)));

  m_joinMUC = new QPushButton(KIconLoader::global()->loadIcon("edit-find-user", KIconLoader::Toolbar, KIconLoader:: SizeSmall), i18n("&Join MUC"), this);
  m_buttonslayout->addWidget(m_joinMUC);
  connect(m_joinMUC, SIGNAL(clicked(bool)), SLOT(joinMUCClicked(bool)));

  m_MUCbookmarks = new QListWidget(this);
  connect(m_MUCbookmarks, SIGNAL(itemDoubleClicked(QListWidgetItem *)), SLOT(bookmarkedMUC(QListWidgetItem *)));
  // finally add to layout
  m_mainlayout->addLayout(m_buttonslayout);
  m_mainlayout->addWidget(m_MUCbookmarks);

  setLayout(m_mainlayout);
}

MUCView::~MUCView()
{
  
}

void MUCView::createMUCClicked(bool toggled)
{
  std::cout << "Create new MUC" << std::endl;
}

void MUCView::joinMUCClicked(bool toggled)
{
  std::cout << "Join MUC" << std::endl;
  bool ok;
  QString jid = QInputDialog::getText(this, i18n("Join existing MUC"), i18n("Conference JID:"), QLineEdit::Normal, "", &ok);
  if (ok && !jid.isEmpty())
  {
    QRegExp re("[^@]+@.+\\..+");
    QRegExpValidator v(re, 0);
    int i;
    if (!(v.validate(jid, i) == QValidator::Acceptable))
    {
      QMessageBox::critical(this, i18n("Error"), i18n("Conference JID is not valid!"));
      return;
    }
    std::cout << "MUC JID: " << jid.toStdString() << std::endl;
  }
}

void MUCView::bookmarkedMUC(QListWidgetItem *item)
{
  std::cout << "Bookmarked MUC" << std::endl;
}
