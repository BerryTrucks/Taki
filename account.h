/* $Id: account.h */
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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "reginfo.h"
#include "transportinfo.h"
#include <QMetaType>

class Account
{
public:
    explicit Account();
    ~Account();

    void loadSettings();
    void saveSettings() const;
    bool active() const;

    QString name() const;
    QString name(int maxLen) const;
    QString nameShort() const;
    QString infoRegistration() const;
    QString infoVoicemail() const;

    int accId() const;
    void setAccId(int);
    void remove() const;

    friend QDebug operator <<(QDebug dbg, const Account &account);

    QString displayName;
    QString userId;
    QString domain;
    bool enabled;
    QString realm;
    QString authName;
    QString userPassword;

    bool sipRegister;
    QString sipRegistrar;
    int regExpiry;
    bool useOutboundProxy;
    QString outbound;

    bool publish;

    QString transport;
    int natKeepAlive;
    bool allow_contact_rewrite;
    bool allow_via_rewrite;
    bool use_rfc5626;
    bool no_stun;

    QString srtp;
    QString srtpReq;
    QString caFile;
    QString privKeyFile;
    QString certFile;
    bool zrtpEnabled;

    int pjSipAccId;
    QString regStatus;
    QString regStatusText;
    RegInfo regInfo;
    TransportInfo trInfo;

    bool mwiStatus;
    int mwiNumNewMsg;
    int mwiNumOldMsg;

    bool callRecAll;
    bool callRecConf;

    bool autoAnswer;

    int ringTone;

    QString contactPlusReplace;
    QString contactPrefix;

    bool mwi;
    QString vmNumber;
    QString vmDTMF;

private:
    int id;             // unique account id
};

Q_DECLARE_METATYPE(Account)

#endif // ACCOUNT_H
