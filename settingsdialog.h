/* $Id: settingsdialog.h */
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "account.h"
#include "QsKineticScroller.h"
#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

class Transport {
public:
    enum Types {
        Auto = 0,
        UDP,
        TCP,
        TLS
    };
};

class UseSrtp
{
public:
    enum usage {
        disabled = 0,
        optional,
        mandatory
    };
};

class SecSignalingSrtp
{
public:
    enum usage {
        none = 0,
        TLS,
        SIPS
    };
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    enum SettingsSections {
        User = 0,
        SIPserver,
        Presence,
        Transport,
        Calls,
        RingTones,
        Dialing,
        Voicemail
    };

    explicit SettingsDialog(Account *ac, const int tabIndex = 0,
                            QWidget *parent = 0);
    ~SettingsDialog();
    Account *getResult() const;

private:
    Ui::SettingsDialog *ui;
#if defined(Q_OS_QNX)
    QsKineticScroller *userScroller;
    QsKineticScroller *configSectionsScroller;
#endif
    Account *ac;

    int transportNameToIndex(QString &tr) const;
    QString transportIndexToName(int idx) const;

    int useSrtpToIndex(QString &tr) const;
    QString useSrtpIndexToName(int idx) const;

private slots:
    void onTestRing(const int ringIndex);

signals:
    void testRing(int ringIndex);
};

#endif // SETTINGSDIALOG_H
