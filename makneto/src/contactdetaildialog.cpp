/*
 * contactdetaildialog.cpp
 *
 * Copyright (C) 2008 Radek Novacek <rad.n@gmail.com>
 * Copyleft 2009-2010 Petr Mensik <pihhan@cipis.net>
 */

#include <xmpp_tasks.h>
#include <QStringList>
#include "contactdetaildialog.h"
#include "maknetocontact.h"

contactDetailDialog::contactDetailDialog(QWidget *parent, QString jid) : KDialog(parent), contactJID(jid)
{
  QWidget *widget = new QWidget(this);
  
  ui.setupUi(widget);
  setMainWidget(widget);

  setCaption(i18n("Add contact"));
  setButtons(KDialog::Ok | KDialog::Cancel);
  setModal(true);

  ui.editJid->setText(jid);
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

void contactDetailDialog::detailsArrived()
{
    XMPP::JT_VCard *req = dynamic_cast<XMPP::JT_VCard *>(sender());

    if (req) {
        
        ui.editNickname->setText( req->vcard().nickName() );
        ui.editBirthDay->setText( 
            req->vcard().bday().toString(Qt::DefaultLocaleShortDate) 
        );
        ui.editFullName->setText( req->vcard().fullName() );

        contactDescription = req->vcard().desc();
        updateDetailText();
        updatePhoto(req->vcard().photo());

        req->safeDelete();
    }
}

void contactDetailDialog::setDetailText(const QString &text)
{
    contactDescription = text;
    updateDetailText();
}

void contactDetailDialog::updateDetailText()
{
    QString full = contactDescription + "\n\n" + featureDescription;
    ui.detailText->setPlainText(full);
}

/** @brief update photo label from image data. */
void contactDetailDialog::updatePhoto(const QByteArray &img)
{
    if (img.size() > 0) {
        QImage image = QImage::fromData(img);
        ui.labelPhoto->setPixmap(QPixmap::fromImage(image));
        ui.labelPhoto->setText(QString());
    } else {
        ui.labelPhoto->setText(i18n("Contact does not have photo."));
        ui.labelPhoto->setPixmap(QPixmap());
    }
}

void contactDetailDialog::describeContact(MaknetoContact *contact)
{
    describeFeatures(contact);
    updateDetailText();
}

void contactDetailDialog::describeFeatures(MaknetoContact *contact)
{
    if (!contact)
        return;
    QString desc;
    MaknetoContact::ResourcesHash hash = contact->allResources();
    for (MaknetoContact::ResourcesHash::const_iterator it = hash.begin();
            it != hash.end(); it++ ) {
        MaknetoContactResource *r = it.value();
        desc += tr("Resource: ") + it.key() + "\n";
        FeatureList *features = r->features();
        if (features) {
            desc += tr("Version: %1").arg(features->ver()) + "\n";
            desc += tr("Node: %1").arg(features->node()) + "\n";
            desc += tr("Supported features:") + "\n";
            QStringList list = features->allFeatures();
            for (ssize_t i=0; i< list.size(); i++) {
                desc += list[i] + "\n";
            }
        } else {
            desc += tr("Entity does not support Entity Capabilitiesi extension.") + "\n";
        }

    }
    ui.featuresText->setPlainText(desc);
}


#include "contactdetaildialog.moc"
