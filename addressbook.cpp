/* $Id: addressbook.cpp */
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

#include "addressbook.h"
#include "contactdetails.h"
#include "contactview.h"
#include "generalsettings.h"
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QDebug>
#include <QString>

#if defined(Q_OS_QNX)
const char *addressBookFile = "data/addressbook.txt.dat";
const char *oldAddressBookFile = "data/addressbook.txt";
#else
const char *addressBookFile = "addressbook.txt.dat";
const char *oldAddressBookFile = "addressbook.txt";
#endif

#ifdef BB10_BLD
template <typename T>
static void updateContactAttribute(ContactBuilder &builder, const Contact &contact,
                                   AttributeKind::Type kind,
                                   AttributeSubKind::Type subKind,
                                   const T &value, int id);
#endif

AddressBook::AddressBook(QTreeWidget *tw, const AddressBookSettings &abs, QObject *parent) :
    QObject(parent),
    settings(abs),
#ifdef BB10_BLD
    m_contactService(new ContactService()),
#endif
    abTreeWidget(tw)
{
    load();

    // Convert previous version format
    if (!importCsv(oldAddressBookFile)) {
        save();
        QFile::rename(oldAddressBookFile, oldAddressBookFile + QString(".bak"));
    }

    abTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
    abTreeWidget->header()->setSortIndicator(0, Qt::AscendingOrder);

#ifdef BB10_BLD
    abTreeWidget->header()->hide();
    // Hide the second columnt (with phone number)
    abTreeWidget->setColumnHidden(1, true);
    connect(abTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
            this, SLOT(openContact(QTreeWidgetItem *, int)));
#endif

#ifndef BB10_BLD
//    abTreeWidget->setUniformRowHeights(false);
#endif

    //generateContacts(1000);
}

AddressBook::~AddressBook()
{
#ifdef BB10_BLD
    delete m_contactService;
#endif
}

void AddressBook::loadContactDetails(Cont &cont) const
{
#ifdef BB10_BLD
    const Contact contact = m_contactService->contactDetails(cont.getId());
    cont.setFirstName(contact.firstName());
    cont.setLastName(contact.lastName());
    cont.setCompany(contact.displayCompanyName());
    cont.setPlatformDisplayName(contact.displayName());
    cont.setPlatformCompanyName(contact.displayCompanyName());
    cont.setPhotoFilepath(contact.smallPhotoFilepath());

    const QList<ContactAttribute> phones = contact.phoneNumbers();
    cont.setPhones(phones);

    if (!phones.isEmpty()) {
        cont.setPhoneNumber(phones.first().value());
        cont.setPhoneNumberType(phones.first().subKind());
    }
#else
    Q_UNUSED(cont);
#endif
}

void AddressBook::openContact(QTreeWidgetItem * item, int column, bool forMsg)
{
    Q_UNUSED(column);

    if (!item)
        return;

    // Load Contact
    QVariant var(item->data(0, Qt::UserRole));
    Cont cont(var.value<Cont>());

#ifdef BB10_BLD
    Q_UNUSED(forMsg);
    loadContactDetails(cont);
    //qDebug() << "Open contact" << cont;

    ContactView cv(&cont);
    connect(&cv, SIGNAL(delContact()), this, SLOT(delContact()));
    connect(&cv, SIGNAL(editContact()), this, SLOT(editContact()));
    cv.setWindowState(cv.windowState() ^ Qt::WindowMaximized ^ Qt::WindowActive);
    int res = cv.exec();

    if (res == QDialog::Accepted) {
        QString number(cont.getPhoneNumber());
        if (!number.isEmpty()) {
            //qDebug() << "Selected contact:" << cont;
            switch (cont.getAction()) {
            case Cont::Dial:
                emit dialContact(cont);
                break;
            case Cont::Copy:
                emit copyContact(cont);
                break;
            case Cont::Message:
            case Cont::CopyMessage:
                emit msgContact(cont);
            }
        }
    }
#else
    if (!cont.getPhoneNumber().isEmpty()) {
        if (forMsg) {
            emit msgContact(cont);
        }
        else {
            emit dialContact(cont);
        }
    }
#endif
}

void AddressBook::load(const QString &searchStr)
{
    QList<QTreeWidgetItem *> items;

    abTreeWidget->clear();

#ifdef BB10_BLD
    QList<Contact> contacts;

    if (searchStr.isEmpty()) {
        ContactListFilters listFilter;
        listFilter.setLimit(0);

        if (settings.displayContOrder == GeneralSettings::displayContactsFLC) {
            listFilter.setSortBy(SortColumn::FirstName, SortOrder::Ascending);
            //qDebug() << "sort by FN";
        }
        else if (settings.displayContOrder == GeneralSettings::displayContactsLFC) {
            listFilter.setSortBy(SortColumn::LastName, SortOrder::Ascending);
            //qDebug() << "sort by LN";
        }
        else if (settings.displayContOrder == GeneralSettings::displayContactsCFL) {
            listFilter.setSortBy(SortColumn::CompanyName, SortOrder::Ascending);
            //qDebug() << "sort by CN";
        }
        else if (settings.displayContOrder == GeneralSettings::displayContactsCLF) {
            listFilter.setSortBy(SortColumn::CompanyName, SortOrder::Ascending);
            //qDebug() << "sort by CN";
        }
        else {
            //qDebug() << "NO sort";
        }
        contacts = m_contactService->contacts(listFilter);
    }
    else {
        ContactSearchFilters searchFilter;
        searchFilter.setSearchValue(searchStr);
        searchFilter.setLimit(0);

        if (settings.displayContOrder == GeneralSettings::displayContactsFLC) {
            searchFilter.setSortBy(SortColumn::FirstName, SortOrder::Ascending);
            //qDebug() << "sort by FN";
        }
        else if (settings.displayContOrder == GeneralSettings::displayContactsLFC) {
            searchFilter.setSortBy(SortColumn::LastName, SortOrder::Ascending);
            //qDebug() << "sort by LN";
        }
        else if (settings.displayContOrder == GeneralSettings::displayContactsCFL) {
            searchFilter.setSortBy(SortColumn::CompanyName, SortOrder::Ascending);
            //qDebug() << "sort by CN";
        }
        else if (settings.displayContOrder == GeneralSettings::displayContactsCLF) {
            searchFilter.setSortBy(SortColumn::CompanyName, SortOrder::Ascending);
            //qDebug() << "sort by CN";
        }
        else {
            //qDebug() << "NO sort";
        }

        contacts = m_contactService->searchContacts(searchFilter);
    }

    foreach (const Contact &idContact, contacts) {
        QTreeWidgetItem *twi;
        Cont cont;
        applyContDisplaySettings(cont);

        cont.setId(idContact.id());
        cont.setPhotoFilepath(idContact.smallPhotoFilepath());


        if (!idContact.firstName().isEmpty()) {
            cont.setFirstName(idContact.firstName());
        }
        if (!idContact.lastName().isEmpty()) {
            cont.setLastName(idContact.lastName());
        }

        // display = sort first = sort last != sort company
        // first or last defined. Best possible guess will be 'first'
        if (idContact.displayName() == idContact.sortFirstName() &&
            idContact.displayName() == idContact.sortLastName()  &&
            idContact.displayName() != idContact.sortCompanyName() )
        {
           cont.setFirstName(idContact.displayName());
           cont.setCompany(idContact.displayCompanyName());
        }
        // display name = display comp = sort last = sort first = sort comp
        // Only company is defined
        else if(idContact.displayName() == idContact.displayCompanyName() &&
                idContact.displayName() == idContact.sortFirstName() &&
                idContact.displayName() == idContact.sortLastName()  &&
                idContact.displayName() == idContact.sortCompanyName())
        {
            cont.setCompany(idContact.displayCompanyName());
        }
        else {
            cont.setFirstName(idContact.sortFirstName());
            cont.setLastName(idContact.sortLastName());
            cont.setCompany(idContact.displayCompanyName());
        }

        // Contact with phone number only
        if (cont.getFirstName().isEmpty() &&
                cont.getLastName().isEmpty() &&
                cont.getCompany().isEmpty())
        {
            cont.setPlatformDisplayName(idContact.displayName());
        }

//        cont.setPlatformDisplayName(idContact.displayName());
//        cont.setPlatformCompanyName(idContact.displayCompanyName());

//        qDebug() << "cont is:" << cont;
//        qDebug() << "cont sortCompanyName is:" << idContact.sortCompanyName();
//        qDebug() << "cont sortFirstName is:" << idContact.sortFirstName();
//        qDebug() << "cont sortLastName is:" << idContact.sortLastName() << "\n";

        twi = prepareItem(cont);
        items.append(twi);
    }
#else
    QString fileName(addressBookFile);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        while (!in.atEnd()) {
            QTreeWidgetItem *twi;
            Cont cont;
            applyContDisplaySettings(cont);

            in >> cont;
            if (!searchStr.isEmpty() && !cont.match(searchStr)) {
                continue;
            }
            twi = prepareItem(cont);
            items.append(twi);
        }
        file.close();
    }
    else {
        qCritical() << "Error opening Addressbook file";
        return;
    }
#endif
    if (!items.isEmpty()) {
        abTreeWidget->insertTopLevelItems(0, items);
    }
}

void AddressBook::applyContDisplaySettings(Cont &cont)
{
    if (settings.displayContOrder == GeneralSettings::displayContactsFLC) {
        cont.setDisplayFormat("FLC");
    }
    else if (settings.displayContOrder == GeneralSettings::displayContactsLFC) {
        cont.setDisplayFormat("LFC");
    }
    else if (settings.displayContOrder == GeneralSettings::displayContactsCFL) {
        cont.setDisplayFormat("CFL");
    }
    else if (settings.displayContOrder == GeneralSettings::displayContactsCLF) {
        cont.setDisplayFormat("CLF");
    }
}

// Expects file in CSV format with lines containing:  name~phone
// If name contans spaces, first word will be saved as FirstName
// the rest part as a LastName
int AddressBook::importCsv(const QString &importFileName, const QString &delimiter)
{
    QFile file(importFileName);

    if (!file.open(QFile::ReadOnly | QFile::Text))
        return -1;

    QTextStream stream(&file);
    QString line;

    do {
        line = stream.readLine();
        if (line.isEmpty())
            continue;

        QStringList pieces(line.split(delimiter));
        if (pieces.count() != 2)
            continue;

        QString fullName(pieces.first());
        QStringList namePieces(fullName.split(" ", QString::SkipEmptyParts));
        Cont cont;
        cont.setFirstName(namePieces.first());
        if (namePieces.length() > 1) {
            namePieces.removeFirst();
            cont.setLastName(namePieces.join(" "));
        }
        cont.setPhoneNumber(pieces.at(1));
        addContactHelper(cont);
    } while (!line.isEmpty());
    file.close();
    return 0;
}

void AddressBook::save()
{
#ifdef BB10_BLD
    // No need to save the list. Use API calls to save individual contacts
    return;
#endif

    /* XXX if search was used it'll save partial list of contacts!!! */
    Cont cont;
    QString fileName(addressBookFile);
    QFile file(fileName);
    QVariant v;

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QDataStream stream(&file);
        for (int i=0; i < abTreeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem *item = abTreeWidget->topLevelItem(i);
            // Get stored Cont
            v = item->data(0, Qt::UserRole);
            cont = v.value<Cont>();
            // Write it
            stream << cont;
        }
        file.close();
    }
    else {
        qCritical() << "Error opening Addressbook file";
    }
}

QString AddressBook::findNameByNumber(const QString &searchNumber) const
{
    Cont cont;
    cont = findContByNumber(searchNumber);
    return cont.getDisplayName();
}

Cont AddressBook::findContByNumber(const QString &searchNumber) const
{
    Cont cont;

#ifdef BB10_BLD
    ContactSearchFilters filter;
    filter.setLimit(1);
    filter.setSearchValue(searchNumber);

    QList<Contact> contacts;
    contacts = m_contactService->searchContactsByPhoneNumber(filter);

    if (!contacts.isEmpty()) {
        cont.setId(contacts.first().id());
        cont.setPlatformDisplayName(contacts.first().displayName());
        cont.setPlatformCompanyName(contacts.first().displayCompanyName());
        cont.setPhotoFilepath(contacts.first().smallPhotoFilepath());
        //loadContactDetails(cont);
    }
#else
    for (int i = 0; i < abTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = abTreeWidget->topLevelItem(i);
        // Get stored Cont
        QVariant v = item->data(0, Qt::UserRole);
        Cont foundCont = v.value<Cont>();

        if (foundCont.getPhoneNumber() == searchNumber) {
            cont = foundCont;
            break;
        }
    }
#endif
    return cont;
}

int AddressBook::addContact(const QString &name, const QString &phone)
{
    Cont cont;
    QStringList nameParts(name.split(" ", QString::SkipEmptyParts));
    if (!nameParts.isEmpty()) {
        QString firstName(nameParts.takeFirst());
        QString lastName(nameParts.join(" "));
        cont.setFirstName(firstName);
        cont.setLastName(lastName);
    }
    cont.setPhoneNumber(phone);

    ContactDetails cd(cont, true);
#if defined(Q_OS_QNX)
    cd.setWindowState(cd.windowState() ^ Qt::WindowMaximized ^ Qt::WindowActive);
#endif
    int res = cd.exec();
    if (res == QDialog::Accepted) {
        addContactHelper(cont);
        save();
    }
    return res;
}

void AddressBook::addContactHelper(Cont &cont)
{
#ifdef BB10_BLD
    ContactBuilder builder;
    builder.addAttribute(ContactAttributeBuilder()
                         .setKind(AttributeKind::Name)
                         .setSubKind(AttributeSubKind::NameGiven)
                         .setValue(cont.getFirstName()));
    builder.addAttribute(ContactAttributeBuilder()
                         .setKind(AttributeKind::Name)
                         .setSubKind(AttributeSubKind::NameSurname)
                         .setValue(cont.getLastName()));
    builder.addAttribute(ContactAttributeBuilder()
                         .setKind(AttributeKind::OrganizationAffiliation)
                         .setSubKind(AttributeSubKind::OrganizationAffiliationName)
                         .setValue(cont.getCompany()));
    builder.addAttribute(ContactAttributeBuilder()
                         .setKind(AttributeKind::Phone)
                         .setSubKind(AttributeSubKind::PhoneMobile)
                         .setValue(cont.getPhoneNumber()));
    Contact createdContact = m_contactService->createContact(builder, false);
    cont.setId(createdContact.id());
    cont.setPhoneNumberType(AttributeSubKind::PhoneMobile);
#endif
    QTreeWidgetItem *twi = prepareItem(cont);
    abTreeWidget->addTopLevelItem(twi);
}

int AddressBook::editContact()
{
    //qDebug() << "Going to edit";
    QTreeWidgetItem *item = abTreeWidget->currentItem();
    if (!item)
        return QDialog::Rejected;

    // Load Contact
    Cont cont;
    QVariant var;
    var = item->data(0, Qt::UserRole);
    cont = var.value<Cont>();
    loadContactDetails(cont);

    // Process Contact edit dialog
    ContactDetails cd(cont, false);
#if defined(Q_OS_QNX)
    cd.setWindowState(cd.windowState() ^ Qt::WindowMaximized ^ Qt::WindowActive);
#endif
    int res = cd.exec();
    if (res == QDialog::Rejected)
        return res;

    //qDebug() << "Contact after edit:" << cont;
    //return res;

    // Save data if Accepted
#ifdef BB10_BLD
    int id = cont.getId();
    //qDebug() << "received contact id" << id;
    if (id) {
        Contact contact = m_contactService->contactDetails(id);
        //qDebug() << "contact id from contactDetails" << contact.id();
        if (contact.id())
        {
            ContactBuilder builder = contact.edit();
            updateContactAttribute<QString>(builder, contact, AttributeKind::Name,
                                            AttributeSubKind::NameGiven,
                                            cont.getFirstName(), -1);
            updateContactAttribute<QString>(builder, contact, AttributeKind::Name,
                                            AttributeSubKind::NameSurname,
                                            cont.getLastName(), -1);
            updateContactAttribute<QString>(builder, contact, AttributeKind::OrganizationAffiliation,
                                            AttributeSubKind::OrganizationAffiliationName,
                                            cont.getCompany(), -1);

            const QList<ContactAttribute> phones = cont.getPhones();
            if (phones.isEmpty()) {
                updateContactAttribute<QString>(builder, contact, AttributeKind::Phone,
                                                cont.getPhoneNumType(), cont.getPhoneNumber(), -1);
            }
            else {
                for (int i = 0; i < phones.size(); ++i) {
                    updateContactAttribute<QString>(builder, contact, AttributeKind::Phone,
                                                    phones.at(i).subKind(), phones.at(i).value(),
                                                    phones.at(i).id());
                }
            }

            //qDebug() << "contact id before update" << contact.id();
            Contact updatedContact = m_contactService->updateContact(builder);
            //qDebug() << "updated contact id" << updatedContact.id();

            // Reload contact to init compnay name
            cont.setId(updatedContact.id());
            loadContactDetails(cont);

        }
    }
#endif
    //qDebug() << "Contact updated";

    // Replace item in the list
    item->setText(0, cont.getDisplayName());
#ifndef BB10_BLD
    item->setText(1, cont.getDisplayPhone());
#endif
    var.setValue(cont);
    item->setData(0, Qt::UserRole, var);

    save();
    return res;
}

int AddressBook::delContact()
{
    QTreeWidgetItem *item = abTreeWidget->currentItem();
    if (!item)
        return 1;

#ifdef BB10_BLD
    Cont cont;
    QVariant var;
    var = item->data(0, Qt::UserRole);
    cont = var.value<Cont>();
    int id = cont.getId();
    if (id) {
        // Remove entire contact
        m_contactService->deleteContact(id);
    }
#endif

    delete item;
    save();
    return 0;
}

QString AddressBook::getContactNumber() const
{
    QTreeWidgetItem *item = abTreeWidget->currentItem();
    Cont cont;
    if (item) {
        QVariant var;
        var = item->data(0, Qt::UserRole);
        cont = var.value<Cont>();
    }
    return cont.getPhoneNumber();
}

QTreeWidgetItem * AddressBook::prepareItem(const Cont &cont) const
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem((QTreeWidget*)0);

    newItem->setText(0, cont.getDisplayName());
#ifndef BB10_BLD
    newItem->setText(1, cont.getDisplayPhone());
    newItem->setTextAlignment(0, Qt::AlignTop);
    newItem->setTextAlignment(1, Qt::AlignTop);
#endif

#ifdef BB10_BLD
    QString iconPath(cont.getPhotoFilepath());
    if (iconPath.isEmpty()) {
        newItem->setIcon(0, QIcon(":/icons/contact.png"));
    }
    else {
        newItem->setIcon(0, QIcon(iconPath));
    }
#endif

    // Save Cont record in the list
    QVariant var;
    var.setValue(cont);
    newItem->setData(0, Qt::UserRole, var);
    return newItem;
}

#ifdef BB10_BLD
/*
 * A helper method to update a single attribute on a Contact object.
 * It first deletes the old attribute (if it exists) and adds the attribute with the
 * new value afterwards.
 * If id specified and >= 0 update one attribute only
*/
template <typename T>
static void updateContactAttribute(ContactBuilder &builder, const Contact &contact,
                                   AttributeKind::Type kind, AttributeSubKind::Type subKind,
                                   const T &value, int id)
{
    // Delete previous instance of the attribute
    QList<ContactAttribute> attributes = contact.filteredAttributes(kind);
    foreach (const ContactAttribute &attribute, attributes) {
        if (attribute.subKind() == subKind) {
            // ID was specifed remove only one requested Attribute
            if (id >= 0) {
                if (attribute.id() == id) {
                    builder.deleteAttribute(attribute);
                }
            }
            else {
                builder.deleteAttribute(attribute);
            }
        }
    }
    // Add new instance of the attribute with new value
    builder.addAttribute(ContactAttributeBuilder()
                         .setKind(kind)
                         .setSubKind(subKind)
                         .setValue(value));
}
#endif

int AddressBook::length() const
{
    return abTreeWidget->topLevelItemCount();
}

void AddressBook::setSettings(const AddressBookSettings &abs)
{
    settings = abs;
}

void AddressBook::generateContacts(int numberOfContacts)
{
    for (int i=0; i< numberOfContacts; ++i) {
        Cont cont;
        cont.setFirstName(QString("FirstName") + QString::number(i));
        cont.setLastName(QString("LastName") + QString::number(i));
        cont.setCompany(QString("Comp") + QString::number(i));
        cont.setPhoneNumber(QString::number(i));
        addContactHelper(cont);
    }
}
