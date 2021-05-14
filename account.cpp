/* $Id: account.cpp */
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

#include "account.h"
#include <QSettings>
#include <QDebug>
#include <QDataStream>
#include <QStringList>

Account::Account() :
    displayName(""),
    userId(""),
    domain(""),
    enabled(true),
    realm(""),
    authName(""),
    userPassword(""),
    sipRegister(false),
    sipRegistrar(""),
    regExpiry(300),
    useOutboundProxy(false),
    outbound(""),
    publish(false),
    transport("UDP"),
    natKeepAlive(20),
    allow_contact_rewrite(false),
    allow_via_rewrite(true),
    use_rfc5626(true),
    no_stun(false),
    srtp("disabled"),
    srtpReq("TLS"),
    caFile(""),
    privKeyFile(""),
    certFile(""),
    zrtpEnabled(false),
    pjSipAccId(-1),
    regStatus(""),
    regStatusText(""),
    mwiStatus(false),
    mwiNumNewMsg(0),
    mwiNumOldMsg(0),
    callRecAll(false),
    callRecConf(false),
    autoAnswer(false),
    ringTone(0),
    contactPlusReplace(""),
    contactPrefix(""),
    mwi(true),
    vmNumber(""),
    vmDTMF(""),
    id(0)
{
}

Account::~Account()
{
}

void Account::loadSettings()
{
    QSettings settings;
    settings.beginGroup("Account/" + QString::number(id));
    displayName = settings.value("displayName").toString();
    userId = settings.value("userId").toString();
    domain = settings.value("domain").toString();
    enabled = settings.value("enabled").toBool();
    realm = settings.value("realm", "").toString();
    authName = settings.value("authName").toString();
    userPassword = settings.value("userPassword").toString();
    sipRegister = settings.value("sipRegister").toBool();
    sipRegistrar = settings.value("sipRegistrar").toString();
    regExpiry = settings.value("regExpiry", 300).toInt();
    useOutboundProxy = settings.value("useOutboundProxy").toBool();
    outbound = settings.value("outbound").toString();
    publish = settings.value("publish").toBool();
    transport = settings.value("transport", "UDP").toString();
    natKeepAlive = settings.value("natKeepAlive", 15).toInt();
    allow_contact_rewrite = settings.value("allow_contact_rewrite", false).toBool();
    allow_via_rewrite = settings.value("allow_via_rewrite", true).toBool();
    use_rfc5626 = settings.value("use_rfc5626", true).toBool();
    no_stun =  settings.value("no_stun", false).toBool();
    srtp = settings.value("srtp", "disabled").toString();
    srtpReq =  settings.value("srtpReq", "TLS").toString();
    zrtpEnabled = settings.value("zrtpEnabled").toBool();
    callRecAll =  settings.value("callRecAll", false).toBool();
    callRecConf =  settings.value("callRecConf", false).toBool();
    autoAnswer = settings.value("autoAnswer", false).toBool();
    ringTone = settings.value("ringTone", 0).toInt();
    contactPlusReplace = settings.value("contactPlusReplace").toString();
    contactPrefix = settings.value("contactPrefix").toString();
    mwi =  settings.value("mwi", true).toBool();
    vmNumber = settings.value("vmNumber").toString();
    vmDTMF = settings.value("vmDTMF").toString();
    settings.endGroup();
}

void Account::saveSettings() const
{
    QSettings settings;
    settings.beginGroup("Account/" + QString::number(id));
    settings.setValue("displayName", displayName);
    settings.setValue("userId", userId);
    settings.setValue("domain", domain);
    settings.setValue("enabled", enabled);
    settings.setValue("realm", realm);
    settings.setValue("authName", authName);
    settings.setValue("userPassword", userPassword);
    settings.setValue("sipRegister", sipRegister);
    settings.setValue("sipRegistrar", sipRegistrar);
    settings.setValue("regExpiry", regExpiry);
    settings.setValue("useOutboundProxy", useOutboundProxy);
    settings.setValue("outbound", outbound);
    settings.setValue("publish", publish);
    settings.setValue("transport", transport);
    settings.setValue("natKeepAlive", natKeepAlive);
    settings.setValue("allow_contact_rewrite", allow_contact_rewrite);
    settings.setValue("allow_via_rewrite", allow_via_rewrite);
    settings.setValue("use_rfc5626", use_rfc5626);
    settings.setValue("no_stun", no_stun);
    settings.setValue("srtp", srtp);
    settings.setValue("srtpReq", srtpReq);
    settings.setValue("zrtpEnabled", zrtpEnabled);
    settings.setValue("callRecAll", callRecAll);
    settings.setValue("callRecConf", callRecConf);
    settings.setValue("autoAnswer", autoAnswer);
    settings.setValue("ringTone", ringTone);
    settings.setValue("contactPlusReplace", contactPlusReplace);
    settings.setValue("contactPrefix", contactPrefix);
    settings.setValue("mwi", mwi);
    settings.setValue("vmNumber", vmNumber);
    settings.setValue("vmDTMF", vmDTMF);
    settings.endGroup();
    settings.sync();
}

void Account::remove() const
{
    QSettings settings;
    settings.beginGroup("Account/" + QString::number(id));
    settings.remove("");
    settings.endGroup();
    settings.sync();
}

bool Account::active() const
{
   if (enabled && !domain.isEmpty() && !userId.isEmpty()) {
       return true;
   }
   return false;
}

QString Account::infoRegistration() const
{
    if (regStatus.isEmpty()) {
        return "";
    }

    QStringList rs;
    rs << regStatus;
    if (!regStatusText.isEmpty()) {
        rs << "\n(" + regStatusText + ")";
    }
    return rs.join(" ");
}

QString Account::infoVoicemail() const
{
    if (!mwiStatus) {
        return "";
    }

    QStringList vmInfo;
    vmInfo << QObject::tr("Voicemail:");
    if (mwiNumNewMsg) {
        vmInfo << QString::number(mwiNumNewMsg) << QObject::tr("new");
    }
    if (mwiNumOldMsg) {
        vmInfo << QString::number(mwiNumOldMsg) << QObject::tr("old");
    }
    return vmInfo.join(" ");
}

// Return Account name suitable for displaying in dropdown lists
QString Account::name() const
{
    if (userId.isEmpty() && domain.isEmpty())
        return "nousername@nodomain";
    else {
        return userId + "@" + domain;

        if (regStatus.isEmpty()) {
            return userId + "@" + domain;
        }
        else {
            QString nameStr = userId + "@" + domain;
            nameStr += " (" + regStatus + ")";
            if (mwiStatus) {
                QStringList vmInfo;
                vmInfo << "\n";
                vmInfo << QObject::tr("Voicemail:");
                if (mwiNumNewMsg) {
                    vmInfo << QString::number(mwiNumNewMsg) << QObject::tr("new");
                }
                if (mwiNumOldMsg) {
                    vmInfo << QString::number(mwiNumOldMsg) << QObject::tr("old");
                }
                nameStr += vmInfo.join(" ");
            }
            return nameStr;
        }
    }
}

QString Account::name(int maxLen) const
{
  QString fullName = name();
  int firstSpace = fullName.indexOf(' ');
  if (firstSpace > 0 && firstSpace < maxLen) {
      return fullName.left(firstSpace);
  }
  return fullName.left(maxLen);
}

QString Account::nameShort() const
{
#define ACCOUNT_NAME_MAX_LENGTH 15
#define ACCOUNT_NAME_LENGTH_DISPLAY 10

    if (userId.isEmpty()) {
        if (domain.isEmpty()) {
            return "nousername";
        }
        else {
            return domain;
        }
    }
    else {
        // Shorten very long names
        if (userId.length() > ACCOUNT_NAME_MAX_LENGTH) {
            QString name = userId.left(ACCOUNT_NAME_LENGTH_DISPLAY);
            name.append("...");
            return name;
        }
        return userId;
    }
}

int Account::accId() const
{
    return id;
}

void Account::setAccId(int accId)
{
    id = accId;
}

QDebug operator<<(QDebug dbg, const Account &ac)
{
    dbg.maybeSpace() << "AccId:" << ac.id << endl;
    dbg.maybeSpace() << "DisplayName:" << ac.displayName << endl;
    dbg.maybeSpace() << "UserId:" << ac.userId << endl;
    dbg.maybeSpace() << "Domain:" << ac.domain << endl;
    dbg.maybeSpace() << "Enabled:" << ac.enabled << endl;
    dbg.maybeSpace() << "pjSipAccId:" << ac.pjSipAccId << endl;
    dbg.maybeSpace() << "regStatus:" << ac.regStatus << endl;
    if (!ac.regStatusText.isEmpty()) {
        dbg.maybeSpace() << "regStatusText:" << ac.regStatusText << endl;
    }
    dbg.maybeSpace() << "MWI:" << ac.mwiStatus << ac.mwiNumNewMsg << ac.mwiNumOldMsg << endl;
    dbg.maybeSpace() << "Transport:" << ac.transport << endl;
    dbg.maybeSpace() << "Reg info:" << ac.regInfo;

    return dbg.maybeSpace();
}
