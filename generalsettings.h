/* $Id: generalsettings.h */
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

#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include <QObject>

class GeneralSettings : public QObject
{
    Q_OBJECT

public:
    enum NetworkInterfaceType {
        NetAny,
        NetWifi,
        NetCellular
    };

    enum DisplayContactsOrder {
        displayContactsFLC,
        displayContactsLFC,
        displayContactsCFL,
        displayContactsCLF
    };

    enum TabSections {
        TabCalls = 0,
        TabDialer,
        TabContacts,
        TabHistory,
        TabAccounts
    };

    explicit GeneralSettings(QObject *parent = 0);
    void save();
    void load();

    // Setting fields
    int codecPriorGsm;
    int codecPriorIlbc;
    int codecPriorG711U;
    int codecPriorG711A;
    int codecPriorSpeexNb;
    int codecPriorSpeexWb;
    int codecPriorSpeexUwb;
    int codecPriorG722;
    bool useStunServer;
    QString stunServer;
    int logLevel;
    int port;
    bool protoUDP;
    bool protoTCP;
    bool protoTLS;
    bool compactFormHeaders;
    int callHistoryMaxCalls;
    int requiredNetworkInterface;
    bool runHeadless;
    int displayContOrder;
    int defaultTab;

    bool tlsVerifyServer;
    bool tlsUseSystemCA;
    bool tlsUseBuiltinCA;
    QString caFile;
    bool tlsVerifyClient;
    QString certFile;
    QString privKeyFile;
    QString privKeyPassword;

signals:

public slots:

};

#endif // GENERALSETTINGS_H
