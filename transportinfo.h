/* $Id: transportinfo.h */
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

#ifndef TRANSPORTINFO_H
#define TRANSPORTINFO_H

#include <pjsua-lib/pjsua.h>
#include <QDebug>
#include <QStringList>
#include <QDateTime>
#include <QObject>
#include <QMetaType>

class SSLCertInfo
{
public:
    SSLCertInfo();
    SSLCertInfo(pj_ssl_cert_info* ssi);

    int version;
    QString subjectCn;
    QString subjectInfo;
    QString issuerCn;
    QString issuerInfo;

    QDateTime validityStart;
    QDateTime validityEnd;
    bool validityGMT;
private:
    void init();
};

class TransportInfo
{
public:
    TransportInfo();
    explicit TransportInfo(pjsip_transport *tp,
                           pjsip_transport_state state,
                           const pjsip_transport_state_info *info);

    friend QDebug operator <<(QDebug dbg, const TransportInfo &ri);

    int stateErrCode;
    QString remoteHost;
    pjsip_transport_state state;
    QString objName;
    QString typeName;
    QString tpInfo;
    unsigned flag;

    bool sslInfo;
    bool sslEstablished;
    QStringList sslProto;
    QString sslCipherName;
    unsigned int sslVerifyStatus;
    QStringList sslVerifyStrings;
    SSLCertInfo sslLocalCert;
    SSLCertInfo sslRemoteCert;
    QString sslLocalCertString;
    QString sslRemoteCertString;

private:
    void init();
};



Q_DECLARE_METATYPE(TransportInfo)

#endif // TRANSPORTINFO_H
