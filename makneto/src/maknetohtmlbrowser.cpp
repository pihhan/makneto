/*
 * maknetohtmlbrowser.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "maknetohtmlbrowser.h"

#include "khtml_part.h"
#include "khtmlview.h"
#include "kurl.h"

#include <QtGui/QVBoxLayout>

MaknetoHTMLBrowser::MaknetoHTMLBrowser(QWidget *)
{
	mainLayout = new QVBoxLayout;

  KUrl url("http://makneto.sourceforge.net/");
	homeTabBrowser = new KHTMLPart;
	homeTabBrowser->openUrl(url);

	mainLayout->addWidget(homeTabBrowser->view());

	setLayout(mainLayout);
}

MaknetoHTMLBrowser::~MaknetoHTMLBrowser()
{
  homeTabBrowser->closeUrl();
  delete homeTabBrowser;
}