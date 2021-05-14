/* $Id: generalsettings.cpp */
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

#include "generalsettings.h"
#include <QSettings>

GeneralSettings::GeneralSettings(QObject *parent) :
    QObject(parent)
{
    load();
}

void GeneralSettings::save()
{
    QSettings settings;
    settings.beginGroup("SIPGeneral");
    settings.setValue("codecPriorGsm", codecPriorGsm);
    settings.setValue("codecPriorIlbc", codecPriorIlbc);
    settings.setValue("codecPriorG711U", codecPriorG711U);
    settings.setValue("codecPriorG711A", codecPriorG711A);
    settings.setValue("codecPriorSpeexNb", codecPriorSpeexNb);
    settings.setValue("codecPriorSpeexWb", codecPriorSpeexWb);
    settings.setValue("codecPriorSpeexUwb", codecPriorSpeexUwb);
    settings.setValue("codecPriorG722", codecPriorG722);
    settings.setValue("useStunServer", useStunServer);
    settings.setValue("stunServer", stunServer);
    settings.setValue("port", port);
    settings.setValue("protoUDP", protoUDP);
    settings.setValue("protoTCP", protoTCP);
    settings.setValue("protoTLS", protoTLS);
    settings.setValue("compactFormHeaders", compactFormHeaders);
    settings.setValue("callHistoryMaxCalls", callHistoryMaxCalls);
    settings.setValue("requiredNetworkInterface", requiredNetworkInterface);
    settings.setValue("runHeadless", runHeadless);
    settings.setValue("displayContOrder", displayContOrder);
    settings.setValue("defaultTab", defaultTab);
    settings.setValue("tlsVerifyServer", tlsVerifyServer);
    settings.setValue("tlsUseSystemCA", tlsUseSystemCA);
    settings.setValue("tlsUseBuiltinCA", tlsUseBuiltinCA);
    settings.setValue("caFile", caFile);
    settings.setValue("tlsVerifyClient", tlsVerifyClient);
    settings.setValue("certFile", certFile);
    settings.setValue("privKeyFile", privKeyFile);
    settings.setValue("privKeyPassword", privKeyPassword);
    settings.endGroup();
    settings.setValue("General/logLevel", logLevel);
    settings.sync();
}

void GeneralSettings::load()
{
    QSettings settings;
    settings.beginGroup("SIPGeneral");
    codecPriorGsm = settings.value("codecPriorGsm",0).toInt();
    codecPriorIlbc = settings.value("codecPriorIlbc",0).toInt();
    codecPriorG711U= settings.value("codecPriorG711U",1).toInt();
    codecPriorG711A = settings.value("codecPriorG711A",0).toInt();
    codecPriorSpeexNb = settings.value("codecPriorSpeexNb",2).toInt();
    codecPriorSpeexWb = settings.value("codecPriorSpeexWb",0).toInt();
    codecPriorSpeexUwb = settings.value("codecPriorSpeexUwb",0).toInt();
    codecPriorG722 = settings.value("codecPriorG722",0).toInt();
    useStunServer = settings.value("useStunServer",false).toBool();
    stunServer = settings.value("stunServer","").toString();
    port = settings.value("port","5060").toInt();
    protoUDP = settings.value("protoUDP",true).toBool();
    protoTCP = settings.value("protoTCP",true).toBool();
    protoTLS = settings.value("protoTLS",false).toBool();
    compactFormHeaders = settings.value("compactFormHeaders",false).toBool();
    callHistoryMaxCalls = settings.value("callHistoryMaxCalls","200").toInt();
    requiredNetworkInterface = settings.value("requiredNetworkInterface",NetAny).toInt();
    runHeadless = settings.value("runHeadless",false).toBool();
    displayContOrder = settings.value("displayContOrder",displayContactsFLC).toInt();
    defaultTab = settings.value("defaultTab",TabDialer).toInt();
    tlsVerifyServer = settings.value("tlsVerifyServer",true).toBool();
    tlsUseSystemCA = settings.value("tlsUseSystemCA",true).toBool();
    tlsUseBuiltinCA = settings.value("tlsUseBuiltinCA",true).toBool();
    caFile = settings.value("caFile","").toString();
    tlsVerifyClient = settings.value("tlsVerifyClient",true).toBool();
    certFile = settings.value("certFile","").toString();
    privKeyFile = settings.value("privKeyFile","").toString();
    privKeyPassword = settings.value("privKeyPassword","").toString();
    settings.endGroup();
    logLevel = settings.value("General/logLevel",3).toInt();
}
