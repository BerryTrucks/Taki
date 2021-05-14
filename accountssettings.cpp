/* $Id: accountsettings.cpp */
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

#include "accountssettings.h"
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QComboBox>
#include <QTreeWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

AccountsSettings::AccountsSettings(QObject *parent) :
    QObject(parent),
    defaultAccountId(0),
    accountIds(""),
    maxAccId(0)
{
}

AccountsSettings::~AccountsSettings()
{
    clearAll();
}

void AccountsSettings::addAccount(Account *ac)
{
    // Generate new Id for account
    maxAccId++;
    int newAccId = maxAccId;
    ac->setAccId(newAccId);

    // Add to the list
    accounts.append(ac);
    // Save
    saveAccounts();
}

void AccountsSettings::deleteAccount(Account *ac)
{
    if (!ac) {
        return;
    }

    for (int i = 0; i < accounts.size(); ++i) {
        if (accounts.at(i)->accId() == ac->accId()) {
            // Clear settings
            ac->remove();
            accounts.removeAt(i);
            delete ac;
            saveAccounts();
            break;
        }
    }
    return;
}

void AccountsSettings::saveAccounts() const
{
    // Save accounts
    QString s;
    QStringList ids;
    for (int i = 0; i < accounts.size(); ++i) {
        accounts.at(i)->saveSettings();
        int id = accounts.at(i)->accId();
        s.setNum(id);
        ids.append(s);
    }
    QString indxStr(ids.join(","));

    // Save indexes, maxId and default accId
    QSettings settings;
    settings.beginGroup("SIPGeneral");
    settings.setValue("accountIds", indxStr);
    settings.setValue("maxAccId", maxAccId);
    settings.setValue("defaultAccountId", defaultAccountId);
    settings.endGroup();
    settings.sync();
}

void AccountsSettings::reloadAccounts()
{
    clearAll();

    QSettings settings;
    settings.beginGroup("SIPGeneral");
    accountIds = settings.value("accountIds", "").toString();
    maxAccId = settings.value("maxAccId", 0).toInt();
    defaultAccountId = settings.value("defaultAccountId", 0).toInt();
    settings.endGroup();

    QStringList ids(accountIds.split(",", QString::SkipEmptyParts));

    for (int i = 0; i < ids.size(); ++i) {
        int accId = ids.at(i).toInt();
        Account *ac = new Account();
        ac->setAccId(accId);
        ac->loadSettings();
        accounts.append(ac);
    }
}

Account * AccountsSettings::getAccount(int idx) const
{
    if (idx < 0 || idx >= accounts.size()) {
        return NULL;
    }
    return accounts.at(idx);
}

Account * AccountsSettings::getEnabledAccount(int idx) const
{
    if (idx < 0 || idx >= accounts.size()) {
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < accounts.size(); ++i) {
        if (accounts.at(i)->enabled) {
            if (idx == j) {
                return accounts.at(i);
            }
            j++;
        }
    }
    return NULL;
}

Account * AccountsSettings::getDefaultAccount() const
{
    Account *ac = NULL;
    for (int i = 0; i < accounts.size(); ++i) {
        ac = accounts.at(i);
        if (ac->accId() == defaultAccountId) {
            return ac;
        }
    }
    // Not found, return the first one if exists
    if (accounts.size()) {
        ac = accounts.at(0);
    }
    return ac;
}

void AccountsSettings::setSelectedAsDefualt(QTreeWidget *tw)
{
    Account *ac;
    ac = getSelectedAccount(tw);
    if (ac) {
        if (ac->enabled) {
            defaultAccountId = ac->accId();
            saveAccounts();
        }
    }
}

Account * AccountsSettings::getSelectedAccount(QTreeWidget *tw) const
{
    int selectedAccId;
    QTreeWidgetItem *curentItem = tw->currentItem();
    if (curentItem) {
        selectedAccId = curentItem->text(ColAccId).toInt();
        return getAccountById(selectedAccId);
    }
    return NULL;
}

Account * AccountsSettings::getPjSipAccount(int acc_id) const
{
    for (int i = 0; i < accounts.size(); ++i) {
        if (accounts.at(i)->pjSipAccId == acc_id)
            return accounts.at(i);
    }
    return NULL;
}

Account * AccountsSettings::getAccountById(int acc_id) const
{
    for (int i = 0; i < accounts.size(); ++i) {
        if (accounts.at(i)->accId() == acc_id)
            return accounts.at(i);
    }
    return NULL;
}

void AccountsSettings::updateAccountWidget(QTreeWidget *tw, Account *ac) const
{
    //initAccountWidget(tw);
    //return;

    QTreeWidgetItemIterator it(tw);
    while (*it) {
        if ((*it)->text(ColAccId).toInt() == ac->accId()) {
            QTreeWidgetItem *twi = prepareItem(*ac);
            if (twi) {
                *(*it) = *twi;
                delete twi;
                tw->resizeColumnToContents(ColAccName);
                tw->resizeColumnToContents(ColAccInfo);
                (*it)->setSizeHint(ColAccName, QSize());
                tw->update();
            }
            break;
        }
        ++it;
    }
}

void AccountsSettings::initAccountWidget(QTreeWidget *tw) const
{
    QMap<QString, Account *> map;
    QList<QTreeWidgetItem *> items;
    QTreeWidgetItem *selectedItem = NULL;
    Account *selectedAcc = NULL;

    // Get account from currently selected Item or use default account
    selectedAcc = getSelectedAccount(tw);
    if (!selectedAcc) {
        selectedAcc = getDefaultAccount();
    }

    // Clear old data
    tw->clear();

    // Sort by Enable and Name
    for (int i = 0; i < accounts.size(); ++i) {
        Account *ac = accounts.at(i);
        map.insert(QString::number(!ac->enabled) + ac->name() + QString::number(ac->accId()), ac);
    }
    foreach (const Account *a, map) {
        QTreeWidgetItem *twi = prepareItem(*a);
        if (selectedAcc) {
            if (a->accId() == selectedAcc->accId()) {
                selectedItem = twi;
            }
        }
        items.append(twi);
    }
    tw->insertTopLevelItems(0, items);

    tw->header()->setSectionHidden(ColAccId, true);

    // Stretch allow other columns to stay on the screen without enablling
    // horizontal scrolling
    tw->header()->setResizeMode(ColAccName, QHeaderView::Stretch);
    // Resize to content streach Account name column moving next columns out of the screen
    //tw->header()->setResizeMode(ColAccName, QHeaderView::ResizeToContents);

    tw->resizeColumnToContents(ColAccName);
#if defined(Q_OS_QNX)
    tw->header()->resizeSection(ColAccVM, 130);
#else
    tw->header()->resizeSection(ColAccVM, 70);
#endif
    tw->resizeColumnToContents(ColAccInfo);

    if (selectedItem) {
        tw->setCurrentItem(selectedItem);
    }
}

QTreeWidgetItem * AccountsSettings::prepareItem(const Account &account) const
{
    QStringList allColumns;
    QStringList col1;

    if (account.enabled) {
        col1 << "<b>" + account.name() + "</b>";
    }
    else {
        col1 << "<font color='grey'>" + account.name();
        col1 << "<br>" + tr("Disabled") + "</font>";
    }

    QString regSt = account.infoRegistration();
    if (!regSt.isEmpty()) {
        col1 << "<br>" + regSt;
    }
    QString vmSt = account.infoVoicemail();
    if (!vmSt.isEmpty()) {
        col1 << "<br>" + vmSt;
    }
    /*
    QString trInfo = account.transportInfo;
    if (!trInfo.isEmpty()) {
        col1 << "<br>" + tr("Transport:") + " " + trInfo;
    }*/

#if defined(Q_OS_QNX)
    // Work around for a resizing bug which doesn't increase raw hight when
    // text wrapping takes place.
    // Add an extra break
    if (!regSt.isEmpty()) {
        col1 << "<br>";
    }
#endif

    allColumns << QString::number(account.accId());
    allColumns << col1.join("");
    allColumns << "";
    allColumns << "";

    QTreeWidgetItem *newItem = new QTreeWidgetItem((QTreeWidget*)0, allColumns);

    // Set account Voicemail icon
    if (account.mwi) {
        newItem->setIcon(ColAccVM, QIcon(":icons/voicemail.png"));
        int numVm = account.mwiNumNewMsg;
        if (numVm) {
            int codeAst = 10033;  // Code for Asterisk
            int codePre1 = 10101; // Code for char before circled 1
            int code = codePre1;
            if (numVm > 0 && numVm < 11) {
                code += numVm;
            }
            else {
                code = codeAst;
            }

#if defined(Q_OS_QNX)
            newItem->setText(ColAccVM, "<span style=\"color:#ff0000; font-size: large;\">&#"
                             + QString::number(code) + ";</span>");
#else
            newItem->setText(ColAccVM, "<span style=\"color:#ff0000; font-size: x-large;\">&#"
                             + QString::number(code) + ";</span>");
#endif
            //newItem->setText(ColAccVM, "&#" + QString::number(code) + ";");
            //newItem->setTextAlignment(ColAccVM, Qt::AlignCenter);
        }
    }
    else {
        // Create disabled icon
        QIcon icon(":icons/voicemail.png");
        QPixmap pm = icon.pixmap(icon.availableSizes().at(0), QIcon::Disabled);
        QIcon iconDisabled(pm);
        newItem->setIcon(ColAccVM, iconDisabled);
        newItem->setText(ColAccVM, "");
    }

    // Set account status icon
    QString iconPath = getAccountIcon(account);
    if (!iconPath.isEmpty()) {
        if (iconPath.startsWith('D')) {
            iconPath.remove(0, 1);
            // Create disabled icon
            QIcon icon(iconPath);
            QPixmap pm = icon.pixmap(icon.availableSizes().at(0), QIcon::Disabled);
            QIcon iconDisabled(pm);
            newItem->setIcon(ColAccInfo, iconDisabled);
        }
        else {
            QIcon icon(iconPath);
            newItem->setIcon(ColAccInfo, icon);
        }
    }
    return newItem;
}

QString AccountsSettings::getAccountIcon(const Account &account) const
{
    QString iconPath("");

    if (account.regInfo.code > 200) {
        iconPath = ":icons/registration-error.png";
    }
    else if (account.regInfo.code == 200) {
        if (account.regInfo.transportFlag & PJSIP_TRANSPORT_RELIABLE) {
            if (account.regInfo.transportFlag & PJSIP_TRANSPORT_SECURE) {
                if (account.regInfo.reason == "OK" && account.trInfo.sslVerifyStatus == 0) {
                    if (account.trInfo.state == PJSIP_TP_STATE_CONNECTED) {
                        iconPath = ":icons/lock-locked.png";
                    }
                    else {
                        iconPath = QString("D") + ":icons/lock-locked.png";
                    }
                }
                else {
                    iconPath = ":icons/lock-unlocked.png";
                }
            }
            else {
                if (account.regInfo.reason == "OK") {
                    if (account.trInfo.state == PJSIP_TP_STATE_CONNECTED) {
                        iconPath = ":icons/registered.png";
                    }
                    else {
                        iconPath = QString("D") + ":icons/registered.png";
                    }
                }
            }
        }
        else if (account.regInfo.reason == "OK") {
            iconPath = ":icons/registered.png";
        }
    }
    else if (account.enabled) {
        // Create disabled-registration icon
        iconPath = QString("D") + ":icons/registered.png";
    }
    return iconPath;
}

bool AccountsSettings::configuredAccounts() const
{
    for (int i = 0; i < accounts.size(); ++i) {
        if (accounts.at(i)->active())
            return true;
    }
    return false;
}

int AccountsSettings::length() const
{
    return accounts.size();
}

void AccountsSettings::clearAll()
{
    defaultAccountId = 0;
    qDeleteAll(accounts.begin(), accounts.end());
    accounts.clear();
}

int AccountsSettings::getNumNewVM() const
{
    int numNewMsg = 0;
    for (int i = 0; i < accounts.size(); ++i) {
        numNewMsg += accounts[i]->mwiNumNewMsg;
    }
    return numNewMsg;
}
