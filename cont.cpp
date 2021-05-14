/* $Id: cont.cpp */
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

#include "cont.h"
#include "cdr.h"
#include "account.h"
#include <QStringList>

Cont::Cont() :
    firstName(""),
    lastName(""),
    company(""),
    phoneNumber(""),
#ifdef BB10_BLD
    phoneNumType(AttributeSubKind::Other),
    phones(),
#endif
    id(0),
    photoFilepath(""),
    platformDisplayName(""),
    platformCompanyName(""),
    action(Dial),
    directCallUri(""),
    pjSipAccId(-1),
    displayFormat("FLC")
{
}

// Build name form First Last name + company on another line
QString Cont::getDisplayName() const
{
    QStringList displayName;
    QStringList allNames;
    bool fnDone = false;
    bool lnDone = false;

    for(int i = 0; i < displayFormat.length(); ++i) {
        if (displayFormat[i] == QChar('F')) {
            if (!firstName.isEmpty()) {
                displayName << firstName;
            }
            fnDone = true;
        }
        if (displayFormat[i] == QChar('L')) {
            if (!lastName.isEmpty()) {
                displayName << lastName;
            }
            lnDone = true;
        }
        if (fnDone && lnDone) {
            if (displayName.isEmpty() && !platformDisplayName.isEmpty()) {
                displayName << platformDisplayName;
            }

            if (!displayName.isEmpty()) {
                allNames << displayName.join(" ");
            }
            fnDone = lnDone = false;
        }

        if (displayFormat[i] == QChar('C')) {
            if (!company.isEmpty()) {
                allNames << company;
            }
            else if (!platformCompanyName.isEmpty()) {
                allNames << platformCompanyName;
            }
        }
    }
    return allNames.join("\n");
}

QString Cont::getDisplayPhone() const
{
    // Convert number to display format
    Cdr cdr;
    cdr.setNumber(phoneNumber);
    QString number(cdr.getDisplayNumber());
    return number;
}

QString Cont::getFirstName() const
{
    return firstName;
}

QString Cont::getLastName() const
{
    return lastName;
}
QString Cont::getCompany() const
{
    return company;
}

QString Cont::getPhoneNumber() const
{
    return phoneNumber;
}

QString Cont::getSecStagePhoneNumber() const
{
    return secStagePhoneNumber;
}

int Cont::getId() const
{
    return id;
}

QString Cont::getPhotoFilepath() const
{
    return photoFilepath;
}

Cont::ContAction Cont::getAction() const
{
    return action;
}

QString Cont::getDirectCallUri() const
{
    return directCallUri;
}

int Cont::getPjSipAccId() const
{
    return pjSipAccId;
}

void Cont::setAction(const ContAction act)
{
    action = act;
}

void Cont::setDisplayFormat(const QString &dispFormat)
{
    displayFormat = dispFormat;
}

#ifdef BB10_BLD
QString Cont::getPlatformDisplayName() const
{
    return platformDisplayName;
}

QString Cont::getPlatformCompanyName() const
{
    return platformCompanyName;
}

QList <ContactAttribute> Cont::getPhones() const
{
    return phones;
}
#endif

void Cont::setFirstName(const QString &fn)
{
    firstName = fn;
}

void Cont::setLastName(const QString &ln)
{
    lastName = ln;
}

void Cont::setCompany(const QString &comp)
{
    company = comp;
}

void Cont::setPhoneNumber(const QString &pn)
{
    phoneNumber = pn;
}

void Cont::setSecStagePhoneNumber(const QString &pn)
{
    secStagePhoneNumber = pn;
}

void Cont::setPhoneNumberAt(const int index, const QString &pn)
{
#ifdef BB10_BLD
    ContactAttribute attr = phones.at(index);
    AttributeSubKind::Type attrSubKind = attr.subKind();
    ContactAttributeBuilder attrBuilder = attr.edit();
    attrBuilder.setSubKind(attrSubKind);
    attrBuilder.setValue(pn);
#else
    Q_UNUSED(index);
    Q_UNUSED(pn);
#endif
}

void Cont::setId(const int i)
{
    id = i;
}

void Cont::setPhotoFilepath(const QString &path)
{
    photoFilepath = path;
}

void Cont::setPjSipAccId(int accId)
{
    pjSipAccId = accId;
}

#ifdef BB10_BLD
void Cont::setPlatformDisplayName(const QString &name)
{
    platformDisplayName = name;
}

void Cont::setPlatformCompanyName(const QString &name)
{
    platformCompanyName = name;
}

void Cont::setPhones(const QList<ContactAttribute> &ph)
{
    phones.clear();
    for (int i = 0; i < ph.size(); ++i) {
        phones.append(ph.at(i));
    }
}

AttributeSubKind::Type Cont::getPhoneNumType() const
{
    return phoneNumType;
}

void Cont::setPhoneNumberType(AttributeSubKind::Type phType)
{
    phoneNumType = phType;
}

// Convert Contact phone attribute to plain text
QString phoneType(const AttributeSubKind::Type ca)
{
    QString phType;
    switch (ca) {
    case AttributeSubKind::Home:
        phType = QObject::tr("Home");
        break;
    case AttributeSubKind::Work:
        phType = QObject::tr("Work");
        break;
    case AttributeSubKind::PhoneMobile:
        phType = QObject::tr("Mobile");
        break;
    case AttributeSubKind::FaxDirect:
        phType = QObject::tr("Fax direct");
        break;
    case AttributeSubKind::Other:
    default:
        phType = QObject::tr("Other");
    }
    return phType;
}
#endif

bool Cont::match(const QString &searchString) const
{
    QRegExp rxSearchStr(searchString, Qt::CaseInsensitive);
    if (rxSearchStr.indexIn(firstName) != -1) {
        return true;
    }
    if (rxSearchStr.indexIn(lastName) != -1) {
        return true;
    }
    if (rxSearchStr.indexIn(company) != -1) {
        return true;
    }
    if (rxSearchStr.indexIn(phoneNumber) != -1) {
        return true;
    }
    return false;
}

QString Cont::buildDirectCallUri(const Account &ac, bool translateNumber)
{
    directCallUri = phoneNumber;

    if (directCallUri.isEmpty()) {
        return directCallUri;
    }

    if (translateNumber) {
        // Replace leading plus according Account settings
        QRegExp rxPlus("^\\+");
        directCallUri.replace(rxPlus, ac.contactPlusReplace);
        // Prepend dial prefix
        directCallUri.prepend(ac.contactPrefix);
    }

    // Remove all extra chars: ()-space
    QRegExp rx("[\\(\\)\\s\\-]");
    directCallUri.replace(rx, QString(""));

    // Remove dots in number, not in domain names
    QRegExp rxDotsAndNumbers("^[\\d\\.]+$");
    if (directCallUri.contains(rxDotsAndNumbers)) {
        QRegExp rxDots("[\\.]");
        directCallUri.replace(rxDots, QString(""));
    }

    directCallUri.remove(QRegExp("^sip:",Qt::CaseInsensitive));
    directCallUri = "sip:" + directCallUri;
    if (!directCallUri.contains("@")) {
        directCallUri = directCallUri + "@" + ac.domain;
    }

    if (!directCallUri.contains(";transport=", Qt::CaseInsensitive)) {
        if ((ac.transport == "UDP") || (ac.transport == "UDP6")) {
            directCallUri.append(";transport=udp");
        }
        else if ((ac.transport == "TCP") || (ac.transport == "TCP6")) {
            directCallUri.append(";transport=tcp");
        }
        else if ((ac.transport == "TLS")) {
            directCallUri.append(";transport=tls");
        }
    }

    return directCallUri;
}

void Cont::copyPlatformProperties()
{
#ifdef BB10_BLD
    if (firstName.isEmpty() && lastName.isEmpty()) {
        firstName = platformDisplayName;
    }
    if (company.isEmpty()) {
        company = platformCompanyName;
    }
#endif
}

QDataStream & operator<<(QDataStream &out, const Cont &c)
{
    return out << c.id << c.firstName << c.lastName << c.company << c.phoneNumber;
}

QDataStream & operator>>(QDataStream &in, Cont &c)
{
    return in >> c.id >> c.firstName >> c.lastName >> c.company >> c.phoneNumber;
}

QDebug operator<<(QDebug dbg, const Cont &c)
{
    dbg.maybeSpace() << "Id:" << c.id << "\n";
    dbg.maybeSpace() << "FirstName:" << c.firstName << "\n";
    dbg.maybeSpace() << "LastName:" << c.lastName << "\n";
    dbg.maybeSpace() << "Company:" << c.company << "\n";
    dbg.maybeSpace() << "PhoneNumber:" << c.phoneNumber << "\n";
    dbg.maybeSpace() << "SecStagePhoneNumber:" << c.secStagePhoneNumber << "\n";
#ifdef BB10_BLD
    dbg.maybeSpace() << "PhoneNumberType:" << phoneType(c.phoneNumType) << "\n";
    for (int i = 0; i < c.phones.size(); ++i) {
        dbg.maybeSpace() << "AttrList PhoneNumber:" << c.phones.at(i).value() << "\n";
        dbg.maybeSpace() << "AttrList PhoneType:" << phoneType(c.phones.at(i).subKind()) << "\n";
    }
    dbg.maybeSpace() << "PhotoFilepath:" << c.photoFilepath << "\n";
    dbg.maybeSpace() << "PlatformDisplayName:" << c.platformDisplayName << "\n";
    dbg.maybeSpace() << "PlatformCompanyName:" << c.platformCompanyName << "\n";
#endif
    dbg.maybeSpace() << "ContAction:" << c.action << "\n";
    return dbg.maybeSpace();
}
