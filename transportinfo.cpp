/* $Id: transportinfo.cpp */
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

#include "transportinfo.h"

TransportInfo::TransportInfo()
{
    init();
}

void TransportInfo::init()
{
    stateErrCode = 0;
    remoteHost = "";
    state = PJSIP_TP_STATE_DISCONNECTED;
    objName = "";
    typeName = "";
    tpInfo = "";
    flag = 0;
    sslInfo = false;
    sslEstablished = false;
    sslProto.clear();
    sslCipherName = "";
    sslVerifyStatus = 0;
    sslVerifyStrings.clear();
    sslLocalCertString = "";
    sslRemoteCertString = "";
}

TransportInfo::TransportInfo(pjsip_transport *tp,
                             pjsip_transport_state st,
                             const pjsip_transport_state_info *info)
{
    init();
    char host_port[128];
    pj_ansi_snprintf(host_port, sizeof(host_port), "[%.*s:%d]",
                         (int)tp->remote_name.host.slen,
                         tp->remote_name.host.ptr,
                         tp->remote_name.port);
    remoteHost = host_port;
    state = st;
    objName = tp->obj_name;
    typeName = tp->type_name;
    tpInfo = tp->info;
    flag = tp->flag;

    stateErrCode = info->status;

    if (!pj_ansi_stricmp(tp->type_name, "tls") && info->ext_info) {
        pjsip_tls_state_info *tlssi = (pjsip_tls_state_info *)info->ext_info;

        sslInfo = true;

        sslEstablished = tlssi->ssl_sock_info->established;

        sslProto.clear();
        if (tlssi->ssl_sock_info->proto & PJ_SSL_SOCK_PROTO_SSL2) {
            sslProto << "SSL2";
        }
        if (tlssi->ssl_sock_info->proto & PJ_SSL_SOCK_PROTO_SSL3) {
            sslProto << "SSL3";
        }
        if (tlssi->ssl_sock_info->proto & PJ_SSL_SOCK_PROTO_TLS1) {
            sslProto << "TLS1.0";
        }
        if (tlssi->ssl_sock_info->proto & PJ_SSL_SOCK_PROTO_TLS1_1) {
            sslProto << "TLS1.1";
        }
        if (tlssi->ssl_sock_info->proto & PJ_SSL_SOCK_PROTO_TLS1_2) {
            sslProto << "TLS1.2";
        }
        if (tlssi->ssl_sock_info->proto & PJ_SSL_SOCK_PROTO_SSL23) {
            sslProto << "SSL23";
        }
        if (tlssi->ssl_sock_info->proto & PJ_SSL_SOCK_PROTO_DTLS1) {
            sslProto << "DTLS1";
        }

        sslCipherName = pj_ssl_cipher_name(tlssi->ssl_sock_info->cipher);

        sslVerifyStatus = tlssi->ssl_sock_info->verify_status;

        const char *verif_msgs[32];
        unsigned verif_msg_cnt;
        pj_ssl_cert_get_verify_status_strings(tlssi->ssl_sock_info->verify_status,
                                              verif_msgs, &verif_msg_cnt);
        for (unsigned i = 0; i < verif_msg_cnt; ++i) {
            sslVerifyStrings << QString(verif_msgs[i]);
        }

        sslLocalCert = SSLCertInfo(tlssi->ssl_sock_info->local_cert_info);
        sslRemoteCert = SSLCertInfo(tlssi->ssl_sock_info->remote_cert_info);

        char buf[2048];
        if (tlssi->ssl_sock_info->local_cert_info) {
            pj_ssl_cert_info_dump(tlssi->ssl_sock_info->local_cert_info, " ", buf, sizeof(buf));
            sslLocalCertString = QString(buf);
        }
        if (tlssi->ssl_sock_info->remote_cert_info) {
            pj_ssl_cert_info_dump(tlssi->ssl_sock_info->remote_cert_info, " ", buf, sizeof(buf));
            sslRemoteCertString = QString(buf);
        }
    }

}

QDebug operator<<(QDebug dbg, const TransportInfo &ti)
{
    dbg.maybeSpace() << "State last error code:" << ti.stateErrCode << endl;

    dbg.maybeSpace() << "Remote host:" << ti.remoteHost << endl;

    switch (ti.state) {
    case PJSIP_TP_STATE_CONNECTED:
        dbg.maybeSpace() << "State: connected" << endl;
        break;
     case PJSIP_TP_STATE_DISCONNECTED:
        dbg.maybeSpace() << "State: disconnected" << endl;
        break;
    case PJSIP_TP_STATE_SHUTDOWN:
        dbg.maybeSpace() << "State: shutdown" << endl;
        break;
    case PJSIP_TP_STATE_DESTROY:
        dbg.maybeSpace() << "State: destroy" << endl;
        break;
    default:
        dbg.maybeSpace() << "State: unknown" << endl;
        break;
    }

    dbg.maybeSpace() << "Object name:" << ti.objName << endl;
    dbg.maybeSpace() << "Type name:" << ti.typeName << endl;
    dbg.maybeSpace() << "Info:" << ti.tpInfo << endl;

    if (ti.flag & PJSIP_TRANSPORT_RELIABLE) {
        dbg.maybeSpace() << "Is reliable" << endl;
    }
    if (ti.flag & PJSIP_TRANSPORT_SECURE) {
        dbg.maybeSpace() << "Is secure" << endl;
    }
    if (ti.flag & PJSIP_TRANSPORT_DATAGRAM) {
        dbg.maybeSpace() << "Is datagram" << endl;
    }

    if (ti.sslInfo) {
        dbg.maybeSpace() << "Secure socket connection is established:" << ti.sslEstablished << endl;
        dbg.maybeSpace() << "SSL proto:" << ti.sslProto.join(" ") << endl;
        dbg.maybeSpace() << "SSL cipher name:" << ti.sslCipherName << endl;
        dbg.maybeSpace() << "SSL verify status:" << ti.sslVerifyStatus << endl;
        dbg.maybeSpace() << "SSL verify strings:" << ti.sslVerifyStrings.join(" ") << endl;
        dbg.maybeSpace() << "SSL local:" << endl << ti.sslLocalCertString << endl;
        dbg.maybeSpace() << "SSL remote:" << endl << ti.sslRemoteCertString << endl;
    }

    return dbg.maybeSpace();
}

SSLCertInfo::SSLCertInfo()
{
    init();
}

SSLCertInfo::SSLCertInfo(pj_ssl_cert_info* ssi)
{
    init();

    if (!ssi) {
        return;
    }

    QString s;
    QStringList sl;

    version = ssi->version;
    subjectCn = QString::fromAscii(ssi->subject.cn.ptr,(int)ssi->subject.cn.slen);
    subjectInfo = QString::fromAscii(ssi->subject.info.ptr,(int)ssi->subject.info.slen);
    issuerCn = QString::fromAscii(ssi->issuer.cn.ptr,(int)ssi->issuer.cn.slen);
    issuerInfo = QString::fromAscii(ssi->issuer.info.ptr,(int)ssi->issuer.info.slen);
    validityStart = QDateTime::fromTime_t(ssi->validity.start.sec);
    validityEnd = QDateTime::fromTime_t(ssi->validity.end.sec);
    validityGMT = ssi->validity.gmt;
}

void SSLCertInfo::init()
{
    version = 0;
    subjectCn = "";
    subjectInfo = "";
    issuerCn = "";
    issuerInfo = "";
    validityGMT = 0;
}
