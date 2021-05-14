/* $Id: accountsettings.h */
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

#ifndef ACCOUNTSSETTINGS_H
#define ACCOUNTSSETTINGS_H

#include "account.h"
#include <QObject>

class QComboBox;
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;

class AccountsSettings : public QObject
{
    Q_OBJECT

public:
    explicit AccountsSettings(QObject *parent = 0);
    ~AccountsSettings();

    void reloadAccounts();
    void saveAccounts() const;
    void addAccount(Account *ac);
    void deleteAccount(Account *ac);

    Account * getAccount(int idx) const;
    Account * getEnabledAccount(int idx) const;
    Account * getDefaultAccount() const;
    void setSelectedAsDefualt(QTreeWidget *tw);
    Account * getSelectedAccount(QTreeWidget *tw) const;
    Account * getPjSipAccount(int acc_id) const;
    Account * getAccountById(int acc_id) const;
    QString getAccountIcon(const Account &account) const;

    void initAccountWidget(QTreeWidget *tw) const;
    void updateAccountWidget(QTreeWidget *tw, Account *ac) const;

    bool configuredAccounts() const;
    int length() const;
    void clearAll();
    int getNumNewVM() const;

    enum AccountColumns {
        ColAccId = 0,
        ColAccName,
        ColAccVM,
        ColAccInfo,
    };

private:
    QList<Account *> accounts;
    int defaultAccountId;
    QString accountIds;
    int maxAccId;

    QTreeWidgetItem * prepareItem(const Account &account) const;

signals:

public slots:

};

#endif // ACCOUNTSSETTINGS_H
