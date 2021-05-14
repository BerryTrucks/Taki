/* $Id: settingsdialog.cpp */
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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
//#include <QDebug>

SettingsDialog::SettingsDialog(Account *account, const int tabIndex,
                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
#if defined(Q_OS_QNX)
    userScroller(new QsKineticScroller(this)),
    configSectionsScroller(new QsKineticScroller(this)),
#endif
    ac(account)
{
    ui->setupUi(this);

    QString accName;
#ifdef BB10_BLD
    // Shorten account name to fit into the screen
    accName = ac->name(30);
#else
    accName = ac->name();
#endif

    ui->accName->setText(accName);
    ui->leDisplayName->setText(ac->displayName);
    ui->leDomain->setText(ac->domain);
    ui->leUserId->setText(ac->userId);
    ui->leRealm->setText(ac->realm);
    ui->leAuthName->setText(ac->authName);
    ui->lePassword->setText(ac->userPassword);
    ui->chkbAccEnabled->setChecked(ac->enabled);
    //ui->chkbDefaultInList->setChecked(ac->defaultInList);
    ui->chkbRegister->setChecked(ac->sipRegister);
    ui->leRegistrar->setText(ac->sipRegistrar);
    ui->leRegExpiry->setValue(ac->regExpiry);
    ui->chkbOutbound->setChecked(ac->useOutboundProxy);
    ui->leOutbound->setText(ac->outbound);
    ui->chkbPublish->setChecked(ac->publish);
    //ui->lePort->setValue(ac->port);
    ui->cbProto->setCurrentIndex(transportNameToIndex(ac->transport));
    ui->leNatKA->setValue(ac->natKeepAlive);
    ui->chkbAllowContactRewrite->setChecked(ac->allow_contact_rewrite);
    ui->chkbAllowViaRewrite->setChecked(ac->allow_via_rewrite);
    ui->chkbUseRFC5626->setChecked(ac->use_rfc5626);
    ui->chkbNoStun->setChecked(ac->no_stun);
    ui->cbUseSrtp->setCurrentIndex(useSrtpToIndex(ac->srtp));
    ui->chkbUseZrtp->setChecked(ac->zrtpEnabled);
    ui->chkbCallRecAll->setChecked(ac->callRecAll);
    ui->chkbCallRecConf->setChecked(ac->callRecConf);
    ui->chkbAutoAnswer->setChecked(ac->autoAnswer);

    ui->cbRingTone->blockSignals(true);
    ui->cbRingTone->setCurrentIndex(ac->ringTone);
    ui->cbRingTone->blockSignals(false);

    ui->leContactPlusReplace->setText(ac->contactPlusReplace);
    ui->leContactPrefix->setText(ac->contactPrefix);

    ui->chkbMWI->setChecked(ac->mwi);
    ui->leVmNumber->setText(ac->vmNumber);
    ui->leVmDTMF->setText(ac->vmDTMF);
#if defined(Q_OS_QNX)
    ui->frDesktopControls->hide();
#else
    ui->frBBControls->hide();
#endif

    if (tabIndex > 0 && tabIndex < ui->listWidget->count()) {
        ui->listWidget->setCurrentItem(ui->listWidget->item(tabIndex));
    }
#if defined(Q_OS_QNX)
    userScroller->enableKineticScrollFor(ui->saSettingsUser);
    configSectionsScroller->enableKineticScrollFor(ui->listWidget);
#endif
}

SettingsDialog::~SettingsDialog()
{
    //delete userScroller;
    delete ui;
}

Account *SettingsDialog::getResult() const
{
    ac->displayName = ui->leDisplayName->text();
    ac->domain = ui->leDomain->text();
    ac->userId = ui->leUserId->text();
    ac->realm = ui->leRealm->text();
    ac->authName = ui->leAuthName->text();
    ac->userPassword = ui->lePassword->text();
    ac->enabled = ui->chkbAccEnabled->checkState();
    //ac->defaultInList = ui->chkbDefaultInList->checkState();
    ac->sipRegister = ui->chkbRegister->checkState();
    ac->sipRegistrar = ui->leRegistrar->text();
    ac->regExpiry = ui->leRegExpiry->value();
    ac->useOutboundProxy = ui->chkbOutbound->checkState();
    ac->outbound = ui->leOutbound->text();
    ac->publish = ui->chkbPublish->checkState();
    //ac->port = ui->lePort->value();
    ac->transport = transportIndexToName(ui->cbProto->currentIndex());
    ac->natKeepAlive = ui->leNatKA->value();
    ac->allow_contact_rewrite = ui->chkbAllowContactRewrite->checkState();
    ac->allow_via_rewrite = ui->chkbAllowViaRewrite->checkState();
    ac->use_rfc5626 = ui->chkbUseRFC5626->checkState();
    ac->no_stun = ui->chkbNoStun->checkState();
    ac->srtp = useSrtpIndexToName(ui->cbUseSrtp->currentIndex());
    ac->zrtpEnabled = ui->chkbUseZrtp->checkState();
    ac->callRecAll = ui->chkbCallRecAll->checkState();
    ac->callRecConf = ui->chkbCallRecConf->checkState();
    ac->autoAnswer = ui->chkbAutoAnswer->checkState();
    ac->ringTone = ui->cbRingTone->currentIndex();
    ac->contactPlusReplace = ui->leContactPlusReplace->text();
    ac->contactPrefix = ui->leContactPrefix->text();
    ac->mwi = ui->chkbMWI->checkState();
    ac->vmNumber = ui->leVmNumber->text();
    ac->vmDTMF = ui->leVmDTMF->text();
    return ac;
}

void SettingsDialog::onTestRing(const int ringIndex)
{
    emit testRing(ringIndex);
}

int SettingsDialog::transportNameToIndex(QString &tr) const
{
    if (tr == "UDP") {
        return Transport::UDP;
    }
    else if (tr == "TCP") {
        return Transport::TCP;
    }
    else if (tr == "TLS") {
        return Transport::TLS;
    }
    return Transport::Auto;
}

QString SettingsDialog::transportIndexToName(int idx) const
{
    QString trName;
    switch (idx) {
    case Transport::Auto:
        trName = "Auto";
        break;
    case Transport::UDP:
        trName = "UDP";
        break;
    case Transport::TCP:
        trName = "TCP";
        break;
    case Transport::TLS:
        trName = "TLS";
        break;
    default:
        break;
    }
    return trName;
}

int SettingsDialog::useSrtpToIndex(QString &tr) const
{
    if (tr == "optional") {
        return UseSrtp::optional;
    }
    else if (tr == "mandatory") {
        return UseSrtp::mandatory;
    }
    return UseSrtp::disabled;
}

QString SettingsDialog::useSrtpIndexToName(int idx) const
{
    QString srtpStr;
    switch (idx) {
    case UseSrtp::optional:
        srtpStr = "optional";
        break;
    case UseSrtp::mandatory:
        srtpStr = "mandatory";
        break;
    case UseSrtp::disabled:
        srtpStr = "disabled";
        break;
    default:
        break;
    }
    return srtpStr;
}
