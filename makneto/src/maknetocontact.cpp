/*
 * maknetocontact.cpp
 *
 * Copyright (C) 2008 Jaroslav Reznik <rezzabuh@gmail.com>
 */

#include "maknetocontact.h"
#include "contactlist/contactlistgroupitem.h"

MaknetoContact::MaknetoContact(const QString& name, const QString& jid, const Status& status, ContactListGroupItem* parent) : ContactListContact(parent), m_name(name), m_jid(jid), m_status(status)
{

}

MaknetoContact::~MaknetoContact()
{

}
