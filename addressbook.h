/* $Id: addressbook.h */
/*
 * Copyright (C) 2012-2014 Dmytro Mishchenko <arkadiamail@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include "cont.h"
#include <QObject>
#include <QTreeWidget>

#ifdef BB10_BLD
#include <bb/pim/contacts/ContactService>
#include <bb/pim/contacts/ContactBuilder>
#include <bb/pim/contacts/ContactAttributeBuilder>
using namespace bb::pim::contacts;
#endif

#define CONTACTDELIMITER "~"

struct AddressBookSettings {
    int displayContOrder;
};

class AddressBook : public QObject
{
    Q_OBJECT

public:
    explicit AddressBook(QTreeWidget *, const AddressBookSettings &abs, QObject *parent = 0);
    ~AddressBook();
    void load(const QString &searchStr = "");
    int importCsv(const QString &importFileName, const QString &delimiter = CONTACTDELIMITER);
    void save();
    QString findNameByNumber(const QString &number) const;
    Cont findContByNumber(const QString &searchNumber) const;

    int addContact(const QString &name = "", const QString &phone = "");
    void addContactHelper(Cont &cont);
    QString getContactNumber() const;
    QTreeWidgetItem * prepareItem(const Cont &cont) const;
    int length() const;
    void setSettings(const AddressBookSettings &ab);

private:
    AddressBookSettings settings;
#ifdef BB10_BLD
    ContactService *m_contactService;
#endif
    QTreeWidget *abTreeWidget;
    void loadContactDetails(Cont &cont) const;
    void applyContDisplaySettings(Cont &cont);
    void generateContacts(int numberOfContacts);

public slots:
    void openContact(QTreeWidgetItem * item, int column, bool forMsg = false);
    int editContact();
    int delContact();

signals:
    void dialContact(const Cont &cont);
    void copyContact(const Cont &cont);
    void msgContact(const Cont &cont);
};

#endif // ADDRESSBOOK_H
