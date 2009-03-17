/*
 * roasterview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "roasterview.h"
#include "makneto.h"
#include "maknetocontactlist.h"
#include "contactlist/contactlistmodel.h"
#include "contactlist/contactlistview.h"
#include "contactlist/contactlist.h"
#include "contactlist/contactlistrootitem.h"
#include "contactlist/contactlistcontact.h"
#include "addcontactdialog.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

#include <klineedit.h>
#include <kdialog.h>


RoasterView::RoasterView(QWidget *, Makneto *makneto): m_makneto(makneto)
{
	m_mainlayout = new QVBoxLayout(this);
	m_buttonslayout = new QHBoxLayout();

	// buttons
	m_addcontact = new QPushButton(KIconLoader::global()->loadIcon("list-add-user", KIconLoader::Toolbar, KIconLoader:: SizeSmall), i18n("&Add contact"), this);
	m_buttonslayout->addWidget(m_addcontact);
	connect(m_addcontact, SIGNAL(clicked(bool)), SLOT(addContactClicked(bool)));

	m_offline = new QPushButton(KIconLoader::global()->loadIcon("edit-find-user", KIconLoader::Toolbar, KIconLoader:: SizeSmall), i18n("&Show all"), this);
  m_offline->setCheckable(true);
	m_offline->setChecked(true);
	m_buttonslayout->addWidget(m_offline);
	connect(m_offline, SIGNAL(clicked(bool)), SLOT(offlineClicked(bool)));
  offlineClicked(true);

	// contact list model
	m_model = new ContactListModel(m_makneto->getContactList());

	// roster
	m_roster = new ContactListView(this);
	m_roster->setModel(m_model);
  m_roster->setIndentation(-3);
  connect(m_roster, SIGNAL(itemDoubleClicked(const QString &)), SLOT(itemDoubleClicked(const QString &)));

	// search button for roster
	m_search = new KLineEdit(this);
	m_search->setClearButtonShown(true);
	connect(m_search, SIGNAL(textChanged(const QString&)), SLOT(search(const QString&)));
	
	// finally add to layout
	m_mainlayout->addLayout(m_buttonslayout);
	m_mainlayout->addWidget(m_search);
	m_mainlayout->addWidget(m_roster);

	setLayout(m_mainlayout);
}

RoasterView::~RoasterView()
{

}

void RoasterView::search(const QString& search)
{
	m_makneto->getContactList()->setSearch(search);
	
}

void RoasterView::addContactClicked(bool /*toggled*/)
{
	AddContactDialog *addContact = new AddContactDialog(this);
	addContact->show();

	connect(addContact, SIGNAL(addUser(const XMPP::Jid &, const QString &, bool)), m_makneto, SLOT(addUser(const XMPP::Jid &, const QString &, bool)));
}

void RoasterView::itemDoubleClicked(const QString &jid)
{
  m_makneto->actionNewSession(jid, Chat);
}

void RoasterView::offlineClicked(bool toggled)
{
	m_makneto->getContactList()->setShowOffline(toggled);
}

#include "roasterview.moc"
