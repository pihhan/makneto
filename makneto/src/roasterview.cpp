/*
 * roasterview.cpp
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "roasterview.h"
#include "makneto.h"
#include "maknetocontactlist.h"
#include "maknetocontact.h"
#include "contactlist/contactlistmodel.h"
#include "contactlist/contactlistview.h"
#include "contactlist/contactlist.h"
#include "contactlist/contactlistrootitem.h"
#include "contactlist/contactlistcontact.h"
#include "contactlist/contactlistgroupedcontact.h"
#include "addcontactdialog.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

#include <klineedit.h>
#include <kdialog.h>
#include <klocalizedstring.h>

#include <QMenu>
#include <QAction>
#include <QMessageBox>


RoasterView::RoasterView(QWidget *, Makneto *makneto): m_makneto(makneto)
{
	m_mainlayout = new QVBoxLayout(this);
	m_buttonslayout = new QHBoxLayout();

	// buttons
	m_addcontact = new QPushButton(KIconLoader::global()->loadIcon("contact-new", KIconLoader::Toolbar), i18n(""), this);
        m_addcontact->setToolTip(i18n("Click to add new contact"));
	m_buttonslayout->addWidget(m_addcontact);
	connect(m_addcontact, SIGNAL(clicked(bool)), SLOT(addContactClicked(bool)));

	m_offline = new QPushButton(KIconLoader::global()->loadIcon("edit-find-user", KIconLoader::Toolbar), i18n(""), this);
        m_offline->setToolTip(i18n("Push to display only connected contacts"));
	m_offline->setCheckable(true);
	m_offline->setChecked(true);
	m_buttonslayout->addWidget(m_offline);
	connect(m_offline, SIGNAL(clicked(bool)), SLOT(offlineClicked(bool)));

        QPixmap audioIcon = KIconLoader::global()->loadIcon(
                "speaker", KIconLoader::Toolbar);
        m_audio = new QPushButton(audioIcon, i18n(""));
        m_audio->setToolTip(i18n("Push to display users can do audio call."));
        m_audio->setCheckable(true);
        m_audio->setChecked(false);
        m_buttonslayout->addWidget(m_audio);
        connect(m_audio, SIGNAL(clicked(bool)), SLOT(audioClicked(bool)) );

        QPixmap videoIcon = KIconLoader::global()->loadIcon(
                "webcamsend", KIconLoader::Toolbar);
        m_video = new QPushButton(videoIcon, i18n(""));
        m_video->setToolTip(i18n("Push to display users can do video call."));
        m_video->setCheckable(true);
        m_video->setChecked(false);
        m_buttonslayout->addWidget(m_video);
        connect(m_video, SIGNAL(clicked(bool)), SLOT(videoClicked(bool)) );

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

    m_roster->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_roster, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
}

RoasterView::~RoasterView()
{

}

void RoasterView::sendMessage()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    if (action)
        m_makneto->contactNewSession(action);
}

void RoasterView::contactDetails()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    if (action)
        m_makneto->contactDetails(action);
}

void RoasterView::audioCall()
{
     QMessageBox::warning(this, "Not implemented", "Nice try, but not yet implemented.");
}

/** @brief Create generic menu common for all contacts. */
QMenu * RoasterView::genericMenu(const QString &jid)
{
        QMenu *menu = new QMenu(this);
        QVariant data(jid);
        QAction *message = menu->addAction(tr("Send message..."));
        message->setData(data);
        connect(message, SIGNAL(triggered()), this, SLOT(sendMessage()) );

        QAction *details = menu->addAction(tr("Details..."));
        details->setData(data);
        connect(details, SIGNAL(triggered()), this, SLOT(contactDetails()) );
        
        return menu;    
}

void RoasterView::showContextMenu(const QPoint &point)
{
    QModelIndex index = m_roster->indexAt(point);
    if (index.isValid()) {
        ContactListItem *item = static_cast<ContactListItem *>(index.internalPointer());
        if (item) {
            ContactListGroupedContact *gc = dynamic_cast<ContactListGroupedContact *>(item);
            MaknetoContact *contact = 0;
            if (gc)
                contact = dynamic_cast<MaknetoContact*> (gc->contact());
            if (contact) {
                QMenu *menu = genericMenu(contact->jid());
                QString jid = contact->jid();


                if (contact->supportsAudio()) {
                    QAction *acall = menu->addAction(tr("&Audio call..."));
                    acall->setData(jid);
                    connect(acall, SIGNAL(triggered()), this, SLOT(audioCall()) );
                }
                if (contact->supportsVideo()) {
                    QAction *acall = menu->addAction(tr("&Video call..."));
                    acall->setData(jid);
                    connect(acall, SIGNAL(triggered()), this, SLOT(audioCall()) );
                }
                if (contact->supportsWhiteboard()) {
                    QAction *acall = menu->addAction(tr("&Whiteboard..."));
                    acall->setData(jid);
                    connect(acall, SIGNAL(triggered()), this, SLOT(audioCall()) );
                }

                QAction *result = menu->exec(m_roster->mapToGlobal(point));
                if (result) {
                    qDebug() << "roster action succesful with action: " << result->text();
                }
            } else {
                QMenu *menu = new QMenu(this);
                menu->addAction("Not a contact, sorry");
                menu->exec(m_roster->mapToGlobal(point));
            }
        }   
    } else {
        qWarning() << "Context menu on roster failed, because at point " << point << " is not valid index.";
    }
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

void RoasterView::audioClicked(bool toggled)
{
    m_makneto->getContactList()->setShowOnlyAudio(toggled);
}

void RoasterView::videoClicked(bool toggled)
{
    m_makneto->getContactList()->setShowOnlyVideo(toggled);
}


#include "roasterview.moc"
