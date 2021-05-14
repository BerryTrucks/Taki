/* $Id: cont.h */
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

#ifndef CONT_H
#define CONT_H

#include <QString>
#include <QMetaType>
#include <QDebug>

#ifdef BB10_BLD
#include <bb/pim/contacts/ContactService>
#include <bb/pim/contacts/ContactBuilder>
#include <bb/pim/contacts/ContactAttributeBuilder>
using namespace bb::pim::contacts;
#endif

#ifdef BB10_BLD
QString phoneType(const AttributeSubKind::Type ca);
#endif

class Account;

class Cont
{
public:
    enum ContAction {
        Dial,
        Copy,
        Message,
        CopyMessage
    };

    Cont();
//    Cont(const Cont &other);
    QString getDisplayName() const;
    QString getDisplayPhone() const;
    QString getFirstName() const;
    QString getLastName() const;
    QString getCompany() const;
    QString getPhoneNumber() const;
    QString getSecStagePhoneNumber() const;
    int getId() const;
    QString getPhotoFilepath() const;
#ifdef BB10_BLD
    QString getPlatformDisplayName() const;
    QString getPlatformCompanyName() const;
    QList <ContactAttribute> getPhones() const;
#endif
    ContAction getAction() const;
    bool match(const QString &searchString) const;
    QString getDirectCallUri() const;
    int getPjSipAccId() const;

    void setFirstName(const QString &fn);
    void setLastName(const QString &ln);
    void setCompany(const QString &comp);
    void setPhoneNumber(const QString &pn);
    void setSecStagePhoneNumber(const QString &pn);
    void setPhoneNumberAt(const int index, const QString &pn);
    void setId(const int id);
    void setPhotoFilepath(const QString &path);
    void setPjSipAccId(int accId);
#ifdef BB10_BLD
    void setPlatformDisplayName(const QString &name);
    void setPlatformCompanyName(const QString &name);
    void setPhones(const QList<ContactAttribute> &ph);
#endif
    void setAction(const ContAction act);
    void setDisplayFormat(const QString &dispFormat);

    QString buildDirectCallUri(const Account &ac, bool translateNumber);
    void copyPlatformProperties();

    friend QDataStream & operator <<(QDataStream& stream, const Cont& contact);
    friend QDataStream & operator >>(QDataStream& stream, Cont& contact);
    friend QDebug operator <<(QDebug dbg, const Cont &contact);

#ifdef BB10_BLD
    AttributeSubKind::Type getPhoneNumType() const;
    void setPhoneNumberType(AttributeSubKind::Type phType);
#endif

private:
    QString firstName;
    QString lastName;
    QString company;
    QString phoneNumber;
    QString secStagePhoneNumber;
#ifdef BB10_BLD
    AttributeSubKind::Type phoneNumType;
    QList<ContactAttribute> phones;
#endif
    int id;
    QString photoFilepath;
    QString platformDisplayName;
    QString platformCompanyName;
    ContAction action;
    QString directCallUri;
    int pjSipAccId;
    QString displayFormat;
};

Q_DECLARE_METATYPE(Cont)

#endif // CONT_H
