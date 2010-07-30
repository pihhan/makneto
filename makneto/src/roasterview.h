/*
 * roasterview.h
 *
 * Copyright (C) 2007 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#ifndef ROASTERVIEW_H
#define ROASTERVIEW_H

#include <QtGui/QWidget>
#include <QMenu>
#include "xmpp.h"
#include "ui_add_contact.h"

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QGridLayout;
class KListWidgetSearchLine;
class QTreeView;
class Makneto;
class ContactListModel;
class ContactListView;
class KLineEdit;

/**
 * This is roaster view widget for Makneto
 *
 * @short Roaster view widget
 * @author Jaroslav Reznik <rezzabuh@gmail.com>
 * @version 0.1
 */

class RoasterView : public QWidget
{
	Q_OBJECT
public:
	/**
	* Default constructor
	*/
	RoasterView(QWidget *parent, Makneto *makneto);

	/**
	* Destructor
	*/
	virtual ~RoasterView();

public slots:
	void search(const QString& search);
	void offlineClicked(bool toggled);
	void addContactClicked(bool toggled);
        void audioClicked(bool toggled);
        void videoClicked(bool toggled);
  void itemDoubleClicked(const QString &);

    void showContextMenu(const QPoint &point);
    void sendMessage();
    void contactDetails();
    void audioCall();


private:
    QMenu *genericMenu(const QString &jid);

	Ui::AddContact ui_add_contact;

	QVBoxLayout *m_mainlayout;
	QHBoxLayout *m_buttonslayout;

	QPushButton *m_addcontact;
	QPushButton *m_offline;
        QPushButton *m_audio;
        QPushButton *m_video;

	Makneto *m_makneto;

	ContactListModel *m_model;
	ContactListView *m_roster;

	KLineEdit *m_search;
};

#endif // ROASTERVIEW_H
