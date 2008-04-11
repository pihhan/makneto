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
#include <QtGui/QFrame>

MaknetoHTMLBrowser::MaknetoHTMLBrowser(QWidget *)
{
	mainLayout = new QVBoxLayout;

	//QFrame *frame = new QFrame;

	KUrl url("http://makneto.sf.net/");
	KHTMLPart *homeTabBrowser = new KHTMLPart;
	homeTabBrowser->openUrl(url);
	//homeTabBrowser->view()->resize(500, 500);

	mainLayout->addWidget(homeTabBrowser->view());

	setLayout(mainLayout);
}
