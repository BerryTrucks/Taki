/* $Id: phone.cpp */
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

#include "phone.h"
#include "projectstrings.h"
#include <QFile>
#include <QTimer>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>

#if defined(Q_OS_QNX)
#ifdef BB10_BLD
#include <bb/platform/NotificationGlobalSettings>
#include <bb/platform/NotificationApplicationSettings>
#include <bps/vibration.h>
#include <bps/led.h>
using namespace bb::platform;
#endif
#endif

/* Ringtones                US         UK  */
#define RINGBACK_FREQ1      440     /* 400 */
#define RINGBACK_FREQ2      480     /* 450 */
#define RINGBACK_ON         2000    /* 400 */
#define RINGBACK_OFF        4000    /* 200 */
#define RINGBACK_CNT        1       /* 2   */
#define RINGBACK_INTERVAL   4000    /* 2000 */
#define RING_FREQ1          800
#define RING_FREQ2          640
#define RING_ON             200
#define RING_OFF            100
#define RING_CNT            3
#define RING_INTERVAL       3000


extern pj_bool_t pjsip_use_compact_form;

extern void *globalPjCallback;

static pjsip_module mod_default_handler =
{
    NULL, NULL,				/* prev, next.		*/
    { "mod-default-handler", 19 },	/* Name.	*/
    -1,					/* Id			*/
    PJSIP_MOD_PRIORITY_APPLICATION+99,	/* Priority	*/
    NULL,				/* load()		*/
    NULL,				/* start()		*/
    NULL,				/* stop()		*/
    NULL,				/* unload()		*/
    PjCallback::default_mod_on_rx_request_wrapper,		/* on_rx_request()	*/
    NULL,				/* on_rx_response()	*/
    NULL,				/* on_tx_request.	*/
    NULL,				/* on_tx_response()	*/
    NULL,				/* on_tsx_state()	*/
};

inline pj_uint8_t codecPriority(int priority)
{
    if (priority) {
        return 20 - priority;
    }
    return 0;
}

Phone::Phone(QObject *parent) :
    QObject(parent),
    phThrHandle(0),
    m_init(false)
{
}

void Phone::init()
{
    if (m_init) {
        return;
    }
    m_init = true;

    pjCallback = NULL;
    settingsGeneral = new GeneralSettings(this);
    settingsAccounts = new AccountsSettings(this);
    sipOn = false;
    confIsRec = false;
    confIsMute = false;
    confIsOnSpeaker = false;
    netAvailable = false;
    p_id = 0;
    player_port = 0;
    isSpeaker = false;
    appVersion = QString(ProjectStrings::userAgent) + " "
            + QString(ProjectStrings::userAgentVersion);

    m_ringTestTimer = new QTimer(this);
    m_ringTestTimer->setInterval(7000);
    m_ringTestTimer->setSingleShot(true);
    connect(m_ringTestTimer, SIGNAL(timeout()), SLOT(onStopTestRing()));

    notifyLed = false;
    notifySound = true;
    notifyVibrate = false;
}

Phone::~Phone() {
    shutdownPjSip();

    qDeleteAll(transports);
    transports.clear();
}

void Phone::logMsg(const QString &logText)
{
    emit logInfo(logText);
}

void Phone::logErrMsg(const QString &logText, pj_status_t status)
{
    char errmsg[PJ_ERR_MSG_SIZE];
    pj_strerror(status, errmsg, sizeof(errmsg));

    QString text(logText);
    text.append(": ");
    text.append(errmsg);

    emit logInfo(text);
}

void Phone::logStatusMsg(const QString &logText, int timeOut)
{
    emit logStatus(logText, timeOut);
}

void Phone::logStatusErrMsg(const QString &logText, pj_status_t status, int timeOut)
{
    char errmsg[PJ_ERR_MSG_SIZE];
    QString text(logText);

    if (status != PJ_SUCCESS) {
        pj_strerror(status, errmsg, sizeof(errmsg));
        text.append(" ");
        text.append(errmsg);
    }
    emit logStatus(text, timeOut);
}

int Phone::initPjSipAccount(int n, Account *ac)
{
    pj_status_t status;

    if (!ac->enabled) {
        return -1;
    }

    logMsg("Init account: " + QString::number(n));

    app_config.accounts[n].ac = ac;
    ac->pjSipAccId = -1;

    pjsua_acc_config_default(&app_config.accounts[n].acc_cfg);

    pjsip_sip_uri sipIdUri;
    pjsip_sip_uri_init(&sipIdUri, PJ_FALSE);

    QString domainName(ac->domain);
    if (domainName.count(QChar(':')) == 1) {
        QStringList domainParts = domainName.split(":");
        domainName = domainParts[0];
        sipIdUri.port = domainParts[1].toInt();
    }

    QByteArray baUserId(ac->userId.toLocal8Bit());
    QByteArray baDomain(domainName.toLocal8Bit());
    sipIdUri.user = pj_str(baUserId.data());
    sipIdUri.host = pj_str(baDomain.data());

    if ((ac->transport == "UDP") || (ac->transport == "UDP6")) {
        sipIdUri.transport_param = pj_str((char *)"udp");
    }
    else if ((ac->transport == "TCP") || (ac->transport == "TCP6")) {
        sipIdUri.transport_param = pj_str((char *)"tcp");
    }
    else if ((ac->transport == "TLS")) {
        sipIdUri.transport_param = pj_str((char *)"tls");
    }

    logMsg("ac transport=" + ac->transport);

    char idUriBuf[PJSIP_MAX_URL_SIZE];
    int len = pjsip_uri_print(PJSIP_URI_IN_REQ_URI, &sipIdUri,
                              idUriBuf, sizeof(idUriBuf));
    idUriBuf[len] = '\0';
    logMsg("sipIdUri=" + QString(idUriBuf));

    app_config.accounts[n].acc_cfg.id.ptr =
            (char*) pj_pool_alloc(app_config.pool, PJSIP_MAX_URL_SIZE);
    if (ac->displayName.isEmpty()) {
        app_config.accounts[n].acc_cfg.id.slen =
                pj_ansi_snprintf(app_config.accounts[n].acc_cfg.id.ptr,
                                 PJSIP_MAX_URL_SIZE, "%s", idUriBuf);
    }
    else {
        QByteArray baDisplayName(ac->displayName.toLocal8Bit());
        app_config.accounts[n].acc_cfg.id.slen =
                pj_ansi_snprintf(app_config.accounts[n].acc_cfg.id.ptr,
                                 PJSIP_MAX_URL_SIZE, "\"%s\"<%s>",
                                 baDisplayName.data(), idUriBuf);
    }

    if (ac->sipRegister) {
        pjsip_sip_uri sipRegUri;
        char regUrlBuf[PJSIP_MAX_URL_SIZE];
        QString regHost;
        QByteArray baRegDomain;

        pjsip_sip_uri_init(&sipRegUri, PJ_FALSE);

        if (ac->sipRegistrar.isEmpty()) {
            sipRegUri.host = sipIdUri.host;
            sipRegUri.port = sipIdUri.port;
        }
        else {
            regHost = ac->sipRegistrar;
            if (regHost.count(QChar(':')) == 1) {
                QStringList regDomainParts = regHost.split(":");
                regHost = regDomainParts[0];
                sipRegUri.port = regDomainParts[1].toInt();
            }
            baRegDomain = regHost.toLocal8Bit();
            sipRegUri.host = pj_str(baRegDomain.data());
        }
        sipRegUri.transport_param = sipIdUri.transport_param;

        len = pjsip_uri_print(PJSIP_URI_IN_REQ_URI, &sipRegUri,
                              regUrlBuf, sizeof(regUrlBuf));
        regUrlBuf[len] = '\0';
        logMsg("regUri=" + QString(regUrlBuf));

        app_config.accounts[n].acc_cfg.reg_uri.ptr =
                (char*) pj_pool_alloc(app_config.pool, PJSIP_MAX_URL_SIZE);
        app_config.accounts[n].acc_cfg.reg_uri.slen =
                pj_ansi_snprintf(app_config.accounts[n].acc_cfg.reg_uri.ptr,
                                 PJSIP_MAX_URL_SIZE, "%s", regUrlBuf);
        app_config.accounts[n].acc_cfg.reg_timeout = ac->regExpiry;
        app_config.accounts[n].acc_cfg.reg_retry_interval = ac->regExpiry;
        app_config.accounts[n].acc_cfg.reg_first_retry_interval = ac->regExpiry;
        logMsg("Registration enabled");
    } else {
        logMsg("Registration disabled");
    }

    QByteArray baRealm;
    if (ac->realm.isEmpty()) {
        QString realm("*");
        baRealm = realm.toLocal8Bit();
    }
    else {
        baRealm = ac->realm.toLocal8Bit();
    }
    app_config.accounts[n].acc_cfg.cred_count = 1;
    app_config.accounts[n].acc_cfg.cred_info[0].realm = pj_str(baRealm.data());
    char digest[] = "digest";
    app_config.accounts[n].acc_cfg.cred_info[0].scheme = pj_str(digest);

    QByteArray baUsername;
    if (ac->authName.isEmpty()) {
        baUsername = ac->userId.toLocal8Bit();
    }
    else {
        baUsername = ac->authName.toLocal8Bit();
    }
    app_config.accounts[n].acc_cfg.cred_info[0].username = pj_str(baUsername.data());
    app_config.accounts[n].acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    QByteArray baPasswd(ac->userPassword.toLocal8Bit());
    app_config.accounts[n].acc_cfg.cred_info[0].data = pj_str(baPasswd.data());

    if (ac->useOutboundProxy) {
        pjsip_sip_uri sipOutboundUri;
        char outboundUrlBuf[PJSIP_MAX_URL_SIZE];
        QString outboundHost;
        QByteArray baOutboundProxy;

        pjsip_sip_uri_init(&sipOutboundUri, PJ_FALSE);

        if (ac->outbound.isEmpty()) {
            sipOutboundUri.host = sipIdUri.host;
            sipOutboundUri.port = sipIdUri.port;
        }
        else {
            outboundHost = ac->outbound;
            if (outboundHost.count(QChar(':')) == 1) {
                QStringList outboundDomainParts = outboundHost.split(":");
                outboundHost = outboundDomainParts[0];
                sipOutboundUri.port = outboundDomainParts[1].toInt();
            }
            baOutboundProxy = outboundHost.toLocal8Bit();
            sipOutboundUri.host = pj_str(baOutboundProxy.data());
        }
        sipOutboundUri.lr_param = 1;
        sipOutboundUri.transport_param = sipIdUri.transport_param;

        len = pjsip_uri_print(PJSIP_URI_IN_REQ_URI, &sipOutboundUri,
                              outboundUrlBuf, sizeof(outboundUrlBuf));
        outboundUrlBuf[len] = '\0';
        logMsg("outboundUri=" + QString(outboundUrlBuf));

        app_config.accounts[n].acc_cfg.proxy_cnt = 1;
        app_config.accounts[n].acc_cfg.proxy[0].ptr =
                (char*) pj_pool_alloc(app_config.pool, PJSIP_MAX_URL_SIZE);
        app_config.accounts[n].acc_cfg.proxy[0].slen =
                pj_ansi_snprintf(app_config.accounts[n].acc_cfg.proxy[0].ptr,
                PJSIP_MAX_URL_SIZE, "%s", outboundUrlBuf);
    }

    if (ac->publish) {
        app_config.accounts[n].acc_cfg.publish_enabled = PJ_TRUE;
        logMsg("Publish enabled");
    } else {
        app_config.accounts[n].acc_cfg.publish_enabled = PJ_FALSE;
        logMsg("Publish disabled");
    }

    if (ac->mwi) {
        app_config.accounts[n].acc_cfg.mwi_enabled = PJ_TRUE;
        logMsg("MWI enabled");
    }
    else {
        app_config.accounts[n].acc_cfg.mwi_enabled = PJ_FALSE;
        logMsg("MWI disabled");
    }

    if (ac->srtp == "disabled") {
        app_config.accounts[n].acc_cfg.use_srtp = PJMEDIA_SRTP_DISABLED;
    } else if (ac->srtp == "optional") {
        app_config.accounts[n].acc_cfg.use_srtp = PJMEDIA_SRTP_OPTIONAL;
    } else if (ac->srtp == "mandatory") {
        app_config.accounts[n].acc_cfg.use_srtp = PJMEDIA_SRTP_MANDATORY;
    }
    if (ac->srtpReq == "none") {
        app_config.accounts[n].acc_cfg.srtp_secure_signaling = 0;
    } else if (ac->srtpReq == "TLS") {
        app_config.accounts[n].acc_cfg.srtp_secure_signaling = 1;
    } else if (ac->srtpReq == "SIPS") {
        app_config.accounts[n].acc_cfg.srtp_secure_signaling = 2;
    }

    if (ac->zrtpEnabled) {
        AccountInfo *ai = new AccountInfo;
        ai->zrtpEnabled = ac->zrtpEnabled;
        app_config.accounts[n].acc_cfg.user_data = ai;
    }
    else {
        app_config.accounts[n].acc_cfg.user_data = NULL;
    }

    app_config.accounts[n].acc_cfg.ka_interval = ac->natKeepAlive;
    app_config.accounts[n].acc_cfg.allow_contact_rewrite = ac->allow_contact_rewrite;
    app_config.accounts[n].acc_cfg.allow_via_rewrite = ac->allow_via_rewrite;
    app_config.accounts[n].acc_cfg.use_rfc5626 = ac->use_rfc5626;
    if (ac->no_stun) {
        app_config.accounts[n].acc_cfg.sip_stun_use = PJSUA_STUN_USE_DISABLED;
        app_config.accounts[n].acc_cfg.media_stun_use = PJSUA_STUN_USE_DISABLED;
    }

    bool defaultAcc = false;
    Account *acDefault = settingsAccounts->getDefaultAccount();
    if (acDefault) {
        if (ac->accId() == acDefault->accId()) {
            defaultAcc = true;
        }
    }

    status = pjsua_acc_add(&app_config.accounts[n].acc_cfg,
                           (defaultAcc) ? PJ_TRUE : PJ_FALSE,
                           &app_config.accounts[n].acc_id);
    if (status != PJ_SUCCESS) {
        logStatusMsg(tr("Failure adding the account. Check the Support window"));
        logErrMsg("Error in pjsua_acc_add", status);
        return status;
    }
    ac->pjSipAccId = app_config.accounts[n].acc_id;

    emit regState(*ac);

    logMsg("Account " + QString::number(n) + " initialized. pjsua_acc_add() succeeded");
    return status;
}

int Phone::initPjSip()
{
    if (sipOn)
        return 0;

    pj_bzero(&app_config, sizeof(app_config));

    logMsg("Init SIP stack");
    logStatusMsg(tr("Initializing"));

    pj_init();
    pj_thread_register("Phone", phThrDesc, &phThrHandle);

    pj_status_t status = pjsua_create();
    if (status != PJ_SUCCESS) {
        logStatusMsg(tr("Initialization error. Please restart the phone or re-save settings"));
        logErrMsg("Error initializing pjsua in pjsua_create().", status);
        return -1;
    }
    logMsg("pjsua_create() succeeded");

    sipOn = true;

    app_config.pool = pjsua_pool_create(ProjectStrings::userAgent, 10000, 5000);
    if (!app_config.pool) {
        logErrMsg("Error in pjsua_pool_create().", 0);
        return -1;
    }
    logMsg("Memory pool created");

    pjsua_config_default(&app_config.cfg);

    app_config.cfg.max_calls = 10;
    app_config.cfg.nat_type_in_sdp = 0;

    FILE *f = fopen("/etc/resolv.conf", "r");
    if (f) {
        while (1) {
            char line[128], server[64];
            int err = fscanf(f, "%126[^\n]\n", line);
            if (err == EOF) {
                break;
            }
            if (err != 1) {
                break;
            }
            //qDebug() << "Line " << line;
            if (1 == sscanf(line, "nameserver %62s\n", &(server[0]))) {
                app_config.cfg.nameserver[app_config.cfg.nameserver_count] =
                        pj_strdup3(app_config.pool, server);
                app_config.cfg.nameserver_count++;
                if (app_config.cfg.nameserver_count == 4) {
                    break;
                }
            }
        }
        fclose(f);
    }

#if 0
    if (app_config.cfg.nameserver_count == 0) {
        logMsg("BB10 method for DNS");
        char *interface;
        if (netstatus_get_default_interface(&interface) == BPS_SUCCESS) {
            netstatus_interface_details_t *details;
            if (netstatus_get_interface_details(interface, &details) == BPS_SUCCESS) {
                int numNS;
                numNS = netstatus_interface_get_num_name_servers(details);
                if (numNS) {
                    const char *ns1 = netstatus_interface_get_name_server(details, 0);
                    app_config.cfg.nameserver[0] = pj_strdup3(app_config.pool, ns1);
                    app_config.cfg.nameserver_count = 1;
                    if ((numNS-1) > 0) {
                        const char *ns2 = netstatus_interface_get_name_server(details, 1);
                        app_config.cfg.nameserver[1] = pj_strdup3(app_config.pool, ns2);
                        app_config.cfg.nameserver_count = 2;
                    }
                }
                netstatus_free_interface_details(&details);
            }
            bps_free(interface);
        }
    }
#endif

    if (app_config.cfg.nameserver_count == 0) {
        QFile file("/pps/services/networking/status_public");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            QRegExp rxNs("^nameservers:json:\\[(.+)\\]$");
            QRegExp rxIP("\"([^:\"]+)\"");

            while(!in.atEnd()) {
                QString ln(in.readLine());
                if (rxNs.indexIn(ln) != -1) {
                    QString ips(rxNs.cap(1));
                    QStringList list;
                    int pos = 0;
                    while ((pos = rxIP.indexIn(ips, pos)) != -1) {
                        list << rxIP.cap(1);
                        pos += rxIP.matchedLength();
                    }
                    for (int i = 0; (i < list.count()) && (i < 4); ++i) {
                        QString dns(list.at(i));
                        QByteArray baDns(dns.toLocal8Bit());
                        app_config.cfg.nameserver[i] = pj_strdup3(app_config.pool, baDns.data());
                        app_config.cfg.nameserver_count++;
                    }
                    break;
                }
            }
            file.close();
        }
    }

    logMsg("cfg.nameserver_count=" + QString::number(app_config.cfg.nameserver_count));
    for (unsigned int count = 0; count < app_config.cfg.nameserver_count; ++count) {
        logMsg("cfg.nameserver[" + QString::number(count) + "]="
               + QString::fromAscii(app_config.cfg.nameserver[count].ptr,
                                    app_config.cfg.nameserver[count].slen));
    }

    if (app_config.cfg.nameserver_count == 0) {
        char dns1[] = "8.8.8.8";
        char dns2[] = "8.8.4.4";
        app_config.cfg.nameserver[0] = pj_strdup3(app_config.pool, dns1);
        app_config.cfg.nameserver[1] = pj_strdup3(app_config.pool, dns2);
        app_config.cfg.nameserver_count = 2;
        logMsg("Using public DNS servers");
    }

    qRegisterMetaType<CallInfo>("CallInfo");
    qRegisterMetaType<MessageRec>("MessageRec");
    qRegisterMetaType<RegInfo>("RegInfo");
    qRegisterMetaType<TransportInfo>("TransportInfo");

    pjCallback = new PjCallback();
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(new_log_message(const QString &)),
                     this, SLOT(logMsg(const QString &)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(setCallState(CallInfo)),
                     this, SLOT(onCallState(CallInfo)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(setMediaState(int, int)),
                     this, SLOT(onMediaState(int, int)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(reg_state(const RegInfo &)),
                     this, SLOT(onRegState(const RegInfo &)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(transport_state(const TransportInfo &)),
                     this, SLOT(onTransportState(const TransportInfo &)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback,
                     SIGNAL(call_transfer_status(int, int, const QString &, bool)),
                     this, SLOT(onCallTransferStatus(int, int, const QString &, bool)),
                     Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(on_pager(MessageRec *)),
                     this, SLOT(onMsgNew(MessageRec *)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback,
                     SIGNAL(on_pager_status(const MessageRec &)),
                     this, SLOT(onMsgStatus(const MessageRec &)),
                     Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(mwi_state(int,bool,int,int)),
                     this, SLOT(onMwiState(int,bool,int,int)), Qt::QueuedConnection);

    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(zrtpStatusSecureOn(int,const QString &)),
                     this, SLOT(onZrtpStatusSecureOn(int,const QString &)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(zrtpStatusSecureOff(int)),
                     this, SLOT(onZrtpStatusSecureOff(int)), Qt::QueuedConnection);
    QObject::connect((PjCallback*)globalPjCallback, SIGNAL(zrtpStatusShowSas(int,const QString &, int)),
                     this, SLOT(onZrtpStatusShowSas(int,const QString &,int)), Qt::QueuedConnection);


    app_config.cfg.cb.on_incoming_call = PjCallback::on_incoming_call_wrapper;
    app_config.cfg.cb.on_call_media_state = PjCallback::on_call_media_state_wrapper;
    app_config.cfg.cb.on_call_state = PjCallback::on_call_state_wrapper;
    app_config.cfg.cb.on_reg_state2 = PjCallback::on_reg_state_wrapper2;
    app_config.cfg.cb.on_buddy_state = PjCallback::on_buddy_state_wrapper;
    app_config.cfg.cb.on_nat_detect = PjCallback::on_nat_detect_wrapper;
    app_config.cfg.cb.on_mwi_info = PjCallback::on_mwi_info_wrapper;
    app_config.cfg.cb.on_call_transfer_status = PjCallback::on_call_transfer_status_wrapper;
    app_config.cfg.cb.on_call_tsx_state = PjCallback::on_call_tsx_state_wrapper;
    app_config.cfg.cb.on_pager2 = PjCallback::on_pager2_wrapper;
    app_config.cfg.cb.on_pager_status2 = PjCallback::on_pager_status2_wrapper;
    app_config.cfg.cb.on_typing2 = PjCallback::on_typing2_wrapper;
    app_config.cfg.cb.on_transport_state = PjCallback::on_transport_state_wrapper;
    app_config.cfg.cb.on_create_media_transport = PjCallback::on_create_media_transport;

    pjsip_use_compact_form = (settingsGeneral->compactFormHeaders) ? PJ_TRUE : PJ_FALSE;

    if (settingsGeneral->useStunServer && !settingsGeneral->stunServer.isEmpty()) {
        QByteArray tempStun(settingsGeneral->stunServer.toLatin1());
        app_config.cfg.stun_srv_cnt = 1;
        app_config.cfg.stun_srv[0] = pj_strdup3(app_config.pool, tempStun.data());
    }

    QByteArray baAppVersion(appVersion.toLocal8Bit());
    pj_strdup2_with_null(app_config.pool, &(app_config.cfg.user_agent), baAppVersion.data());

    pjsua_logging_config_default(&app_config.log_cfg);
    app_config.log_cfg.msg_logging = true;
    app_config.log_cfg.console_level = settingsGeneral->logLevel;
    app_config.log_cfg.cb = PjCallback::logger_cb_wrapper;
    app_config.log_cfg.decor = app_config.log_cfg.decor & ~PJ_LOG_HAS_NEWLINE;
    app_config.log_cfg.decor = app_config.log_cfg.decor | PJ_LOG_HAS_THREAD_ID;

    pjsua_media_config_default(&app_config.media_cfg);
    app_config.media_cfg.no_vad = PJ_TRUE;

#if 1
    if (settingsGeneral->codecPriorG722 ||
        settingsGeneral->codecPriorSpeexWb ||
        settingsGeneral->codecPriorSpeexUwb)
    {
        if ( ! ((settingsGeneral->codecPriorG722 > 0 &&
                 settingsGeneral->codecPriorG722 < 3) ||
                (settingsGeneral->codecPriorSpeexWb > 0 &&
                 settingsGeneral->codecPriorSpeexWb < 3) ||
                (settingsGeneral->codecPriorSpeexUwb > 0 &&
                 settingsGeneral->codecPriorSpeexUwb < 3)) )
        {
            app_config.media_cfg.quality = 4;
            logMsg("resampling filter set to 4");
        }
        logMsg("use default clock rate");
    }
    else {
        app_config.media_cfg.clock_rate = 8000;
    }
#endif

    app_config.cfg.enable_unsolicited_mwi = PJ_FALSE;

    status = pjsua_init(&app_config.cfg, &app_config.log_cfg, &app_config.media_cfg);
    if (status != PJ_SUCCESS) {
        logStatusMsg(tr("Initialization error. Please restart the phone"));
        logErrMsg("Error in pjsua_init()", status);
        shutdownPjSip();
        return -1;
    }
    logMsg("pjsua_init() succeeded");

    status = pjsip_endpt_register_module(pjsua_get_pjsip_endpt(), &mod_default_handler);
    if (status != PJ_SUCCESS) {
        logStatusMsg(tr("Initialization error. Please restart the phone"));
        logErrMsg("Error in pjsip_endpt_register_module()", status);
        shutdownPjSip();
        return -1;
    }
    logMsg("pjsip_endpt_register_module() succeeded");

    ringCreate();


    if (!settingsGeneral->protoUDP &&
        !settingsGeneral->protoTCP &&
        !settingsGeneral->protoTLS)
    {
        logStatusMsg(tr("No Transports configured. Please check Settings"));
        logMsg("No Transports configured.");
        shutdownPjSip();
        return -1;
    }

    if (settingsGeneral->protoUDP) {
        pjsua_transport_config_default(&app_config.udp_cfg);
        app_config.udp_cfg.port = settingsGeneral->port;
        app_config.udp_cfg.qos_type = PJ_QOS_TYPE_VOICE;
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &app_config.udp_cfg,
                                        &app_config.udp_transport_id);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error creating UDP transport", status);
            logStatusMsg(tr("Error creating UDP transport"));
            shutdownPjSip();
            return -1;
        }
        logMsg("UDP transport created");
    }
    if (settingsGeneral->protoTCP) {
        pjsua_transport_config_default(&app_config.tcp_cfg);
        app_config.tcp_cfg.port = settingsGeneral->port;
        app_config.tcp_cfg.qos_type = PJ_QOS_TYPE_VOICE;
        status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &app_config.tcp_cfg,
                                        &app_config.tcp_transport_id);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error creating TCP transport", status);
            logStatusMsg(tr("Error creating TCP transport"));
            shutdownPjSip();
            return -1;
        }
        logMsg("TCP transport created");
    }
    if (settingsGeneral->protoTLS) {
        pjsua_transport_config_default(&app_config.tls_cfg);
        app_config.tls_cfg.port = settingsGeneral->port + 1;
        app_config.tls_cfg.qos_type = PJ_QOS_TYPE_VOICE;
        app_config.tls_cfg.tls_setting.method = PJSIP_TLSV1_METHOD;
        app_config.tls_cfg.tls_setting.proto =
                PJ_SSL_SOCK_PROTO_TLS1 | PJ_SSL_SOCK_PROTO_TLS1_1 | PJ_SSL_SOCK_PROTO_TLS1_2;
        app_config.tls_cfg.tls_setting.verify_server = settingsGeneral->tlsVerifyServer;
        app_config.tls_cfg.tls_setting.verify_client = settingsGeneral->tlsVerifyClient;
        app_config.tls_cfg.tls_setting.require_client_cert = settingsGeneral->tlsVerifyClient;

        QByteArray foundCAPath;
        if (settingsGeneral->tlsUseSystemCA) {
            QList<QByteArray> caPaths;
            caPaths << "/etc/ssl/certs/"         // Linux
                    << "/etc/openssl/certs/";    // Blackberry
            foreach (QByteArray path, caPaths) {
                QDir caDir(path);
                if (caDir.exists()) {
                    foundCAPath = path;
                    break;
                }
            }
            if (!foundCAPath.isEmpty()) {
                logMsg(QString("Found default CA path:") + foundCAPath);
                app_config.tls_cfg.tls_setting.ca_list_path = pj_str(foundCAPath.data());
            }
        }

        QByteArray tempca;
        if (settingsGeneral->tlsUseBuiltinCA) {
            if (rootCAcert.open()) {
                QFile rcCAfile(":/cert/ca-bundle.crt");
                if (rcCAfile.open(QIODevice::ReadOnly)) {
                    rootCAcert.write(rcCAfile.readAll());
                    rcCAfile.close();
                    rootCAcert.close();
                    tempca = rootCAcert.fileName().toLatin1();
                }
                else {
                    logMsg("Error opening CAroot resource file");
                }
            }
            else {
                logMsg("Error creating temp file for CAroot");
            }
            app_config.tls_cfg.tls_setting.ca_list_file = pj_str(tempca.data());
        }

        if (!settingsGeneral->caFile.isEmpty()) {
            tempca = settingsGeneral->caFile.toLatin1();
            app_config.tls_cfg.tls_setting.ca_list_file = pj_str(tempca.data());
        }

        QByteArray tempkey, temppasswd, tempcert;
        tempkey = settingsGeneral->privKeyFile.toLatin1();
        app_config.tls_cfg.tls_setting.privkey_file = pj_str(tempkey.data());

        tempcert = settingsGeneral->certFile.toLatin1();
        app_config.tls_cfg.tls_setting.cert_file = pj_str(tempcert.data());

        temppasswd = settingsGeneral->privKeyPassword.toLatin1();
        app_config.tls_cfg.tls_setting.password = pj_str(temppasswd.data());

        status = pjsua_transport_create(PJSIP_TRANSPORT_TLS, &app_config.tls_cfg,
                                        &app_config.tls_transport_id);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error creating TLS transport", status);
            logStatusMsg(tr("Error creating TLS transport"));
            shutdownPjSip();
            return -1;
        }
        logMsg("TLS transport created");
    }

    int i = 0;
    Account *acnt;
    while ((acnt = settingsAccounts->getEnabledAccount(i))) {
        if (!acnt->domain.isEmpty()) {
            if (pjsua_acc_get_count() < PJSUA_MAX_ACC) {
                initPjSipAccount(i, acnt);
            }
            else {
                logStatusMsg(tr("Too many accounts. Ignoring: ") + acnt->name());
            }
        }
        ++i;
    }

    pj_str_t id;
    pjsua_codec_set_priority(pj_cstr(&id, "PCMA/8000"), codecPriority(settingsGeneral->codecPriorG711A));
    pjsua_codec_set_priority(pj_cstr(&id, "PCMU/8000"), codecPriority(settingsGeneral->codecPriorG711U));
    pjsua_codec_set_priority(pj_cstr(&id, "GSM/8000"), codecPriority(settingsGeneral->codecPriorGsm));
    pjsua_codec_set_priority(pj_cstr(&id, "G722/16000"), codecPriority(settingsGeneral->codecPriorG722));
    pjsua_codec_set_priority(pj_cstr(&id, "iLBC/8000"), codecPriority(settingsGeneral->codecPriorIlbc));
    pjsua_codec_set_priority(pj_cstr(&id, "speex/8000"), codecPriority(settingsGeneral->codecPriorSpeexNb));
    pjsua_codec_set_priority(pj_cstr(&id, "speex/32000"), codecPriority(settingsGeneral->codecPriorSpeexUwb));
    pjsua_codec_set_priority(pj_cstr(&id, "speex/16000"), codecPriority(settingsGeneral->codecPriorSpeexWb));

    logMsg("calling pjsua_start()");

    status = pjsua_start();
    if (status != PJ_SUCCESS) {
        logStatusMsg(tr("Initialization error. Please restart the phone"));
        logErrMsg("Error in pjsua_start()", status);
        shutdownPjSip();
        return -1;
    }
    logMsg("pjsua_start() succeeded");
    return 0;
}

int Phone::shutdownPjSip()
{
    pj_status_t status;
    unsigned int i;

    if (!sipOn)
        return 1;

    sipOn = false;

    logMsg("Shutting down SIP stack");

    globalPjCallback = 0;

    onStopWavPlayback();

    onStopTestRing();

    if (app_config.ringback_port &&
        app_config.ringback_slot != PJSUA_INVALID_ID)
    {
        pjsua_conf_remove_port(app_config.ringback_slot);
        app_config.ringback_slot = PJSUA_INVALID_ID;
        pjmedia_port_destroy(app_config.ringback_port);
        app_config.ringback_port = NULL;
    }

    if (app_config.ring_port && app_config.ring_slot != PJSUA_INVALID_ID) {
        pjsua_conf_remove_port(app_config.ring_slot);
        app_config.ring_slot = PJSUA_INVALID_ID;
        pjmedia_port_destroy(app_config.ring_port);
        app_config.ring_port = NULL;
    }

    for (i = 0; i < app_config.tone_count; ++i) {
        pjsua_conf_remove_port(app_config.tone_slots[i]);
    }

    for (i = 0; i < MAX_ACCOUNTS; ++i) {
        if (app_config.accounts[i].ac) {
            if (app_config.accounts[i].acc_cfg.user_data) {
                delete static_cast<AccountInfo *>(app_config.accounts[i].acc_cfg.user_data);
            }
        }
    }

    if (app_config.pool) {
        pj_pool_release(app_config.pool);
        app_config.pool = NULL;
    }

    if (netAvailable) {
        status = pjsua_destroy();
    }
    else {
        status = pjsua_destroy2(PJSUA_DESTROY_NO_NETWORK);
    }
    if (status != PJ_SUCCESS) {
        logErrMsg("Error in pjsua_destroy()", status);
    }
    else {
        logMsg("pjsua_destroy() succeeded");
    }
    pj_bzero(&app_config, sizeof(app_config));

    pj_shutdown();

    if (pjCallback) {
        pjCallback->disconnect();
        delete pjCallback;
    }
    return 0;
}

void Phone::onReloadPhone(bool newNetStatus)
{
    init();

    netAvailable = newNetStatus;

    if (!configReload.tryLock()) {
        qDebug() << "Can't obtain lock. Another reload is in progress";
        return;
    }

    int ret = shutdownPjSip();

    int timer = (ret) ? 10 : 1000;
    QTimer::singleShot(timer, this, SLOT(startPhone()));
}

void Phone::startPhone()
{
    settingsGeneral->load();
    settingsAccounts->reloadAccounts();

    if (netAvailable) {
        int ret = initPjSip();
        if (!ret) {
            logStatusMsg(tr("Ready"));
        }
    }
    else {
        logStatusMsg(tr("Network is not available."));
    }
    configReload.unlock();
}

void Phone::onSendDTMF(int callId, const QString &key)
{
    QByteArray baKey(key.toLocal8Bit());
    pj_str_t pjDtmf = pj_str(baKey.data());
    pjsua_call_dial_dtmf(callId, &pjDtmf);
}

pj_status_t Phone::activeCall(pjsua_call_id *current_call)
{
    pjsua_call_id call_ids[PJSUA_MAX_CALLS];
    unsigned call_cnt = PJ_ARRAY_SIZE(call_ids);
    unsigned i;

    pjsua_enum_calls(call_ids, &call_cnt);

    for (i=0; i<call_cnt; ++i) {
        if (pjsua_call_is_active(call_ids[i])) {
            *current_call = call_ids[i];
            return PJ_SUCCESS;
        }
    }
    return -1;
}

void Phone::onCallContact(const Cont &cont, int transfereeCallId)
{
    if (!sipOn) {
        logStatusMsg(tr("Phone is offline"), 10000);
        return;
    }

    QString directCallUri(cont.getDirectCallUri());
    if (directCallUri.isEmpty()) {
        logMsg("Phone number is empty");
        return;
    }

    logStatusMsg(tr("Calling"), 5000);

    pjsua_msg_data msg_data;
    pjsua_msg_data_init(&msg_data);

    int call_id;
    QByteArray temp(directCallUri.toLatin1());
    pj_str_t pjuri = pj_str(temp.data());
    logMsg("onCallContact Phone. Uri: " + directCallUri +
           " pjsip acc:" + QString::number(cont.getPjSipAccId()));

    pj_status_t status = pjsua_call_make_call(cont.getPjSipAccId(), &pjuri, 0, 0, &msg_data, &call_id);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error making call", status);
        logStatusErrMsg(tr("Error making call."), status);
        return;
    }

    pj_bzero(&app_config.call_data[call_id], sizeof(call_data));

    qDebug() << "New call id" << call_id << "Set TRANSFEREE CALLID" << transfereeCallId;
    if (transfereeCallId != -1) {
        app_config.call_data[call_id].isTransferor = true;
        app_config.call_data[call_id].transfereeCallId = transfereeCallId;
    }

    if (! cont.getSecStagePhoneNumber().isEmpty()) {
        QByteArray baNumber = cont.getSecStagePhoneNumber().toLocal8Bit();
        strncpy(app_config.call_data[call_id].secStagePhoneNumber,
                baNumber.data(),
                sizeof(app_config.call_data[call_id].secStagePhoneNumber));
    }
}

void Phone::onCallBtnClick(const QString &btn, int callId, bool canResend)
{
    qDebug() << "onCallBtnClick Phone:" << btn << callId;
    logMsg("onCallBtnClick Phone: " + btn + " for call: " + QString::number(callId));

    pj_status_t status;
    bool resend = false;

    if (!sipOn) {
        logStatusMsg(tr("Phone is offline"), 10000);
        return;
    }

    pjsua_call_get_info(callId, &app_config.call_data[callId].call_info);

    if (btn == "btHangup" || btn == "btHangupC" || btn == "btHangupO" || btn == "btReject")
    {
        logStatusMsg(tr("Call ending..."));
        stopNotify(callId);
        status = pjsua_call_hangup(callId, 0, 0, 0);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error in pjsua_call_hangup", status);
            resend = true;
        }
    }
    else if (btn == "btSilence") {
        stopNotify(callId);
    }
    else if (btn == "btHold") {
        logStatusMsg(tr("Putting call on hold"), 5000);
        status = pjsua_call_set_hold(callId, 0);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error pjsua_call_set_hold", status);
        }
    }
    else if (btn == "btUnHold") {
        logStatusMsg(tr("Resuming call"), 5000);
        unHold(callId);
    }
    else if (btn == "btMute" || btn == "btMuteC") {
        if (app_config.call_data[callId].call_info.conf_slot != PJSUA_INVALID_ID) {
            if (app_config.call_data[callId].isInConf)
                setConfMute(callId, true);
            else {
                status = pjsua_conf_disconnect(0, app_config.call_data[callId].call_info.conf_slot);
                if (status != PJ_SUCCESS) {
                    logErrMsg("Error pjsua_conf_disconnect", status);
                }
            }
        }
        else {
            app_config.call_data[callId].deferedMute = true;
            logMsg("defered mute on");
        }
    }
    else if (btn == "btUnMute") {
        if (app_config.call_data[callId].call_info.conf_slot != PJSUA_INVALID_ID) {
            if (app_config.call_data[callId].isInConf)
                setConfMute(callId, false);
            else {
                status = pjsua_conf_connect(0, app_config.call_data[callId].call_info.conf_slot);
                if (status != PJ_SUCCESS) {
                    logErrMsg("Error pjsua_conf_connect", status);
                }
            }
        }
        else {
            app_config.call_data[callId].deferedMute = false;
            logMsg("defered mute off");
        }
    }
    else if (btn == "btAnswer") {
        logStatusMsg(tr("Answering call"), 5000);
        stopNotify(callId);
        status = pjsua_call_answer(callId, 200, 0, 0);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error pjsua_call_answer", status);
            resend = true;
        }
    }
    else if (btn == "btRecord") {
        logStatusMsg(tr("Call recording started"), 5000);
        setRecording(callId, true);
    }
    else if (btn == "btPauseRecord") {
        logStatusMsg(tr("Call recording stopped"), 5000);
        setRecording(callId, false);
    }
#ifdef BB10_BLD
    else if (btn == "btSpeaker") {
        if (app_config.call_data[callId].call_info.conf_slot != PJSUA_INVALID_ID) {
            speakerOn();
        }
        else if (app_config.ringback_cnt) {
            speakerOn();
            app_config.call_data[callId].deferedSpeaker = true;
            logMsg("speaker on for ring, defered speaker on");
        }
        else {
            app_config.call_data[callId].deferedSpeaker = true;
            logMsg("defered speaker on");
        }
    }
    else if (btn == "btHandset") {
        if (app_config.call_data[callId].call_info.conf_slot != PJSUA_INVALID_ID) {
            speakerOff();
        }
        else if (app_config.ringback_cnt) {
            speakerOff();
            app_config.call_data[callId].deferedSpeaker = false;
            logMsg("speaker off for ring, defered speaker off");
        }
        else {
            app_config.call_data[callId].deferedSpeaker = false;
            logMsg("defered speaker off");
        }
    }
#endif
    else if (btn == "btDialpad" || btn == "btDialpadC") {
        qDebug() << "Dialpad";
    }
    else if (btn == "btTransfer") {
        qDebug() << "Getting ready to Transfer a call";

        if (app_config.call_data[callId].isTransferor &&
                app_config.call_data[callId].transfereeCallId != -1)
        {
            attnXfer(app_config.call_data[callId].transfereeCallId, callId);
        }
        else {
            status = pjsua_call_set_hold(callId, 0);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_call_set_hold", status);
            }
            emit getTransfereeNumber(callId);
        }
    }
    else if (btn == "btZrtpConfirm") {
        qDebug() << "confirm sas for" << callId;
        pjmedia_transport *tp = (pjmedia_transport *)pjsua_call_get_user_data(callId);
        if (tp) {
            ZrtpContext* ctx = pjmedia_transport_zrtp_getZrtpContext(tp);
            if (ctx) {
                zrtp_SASVerified(ctx);
                emit zrtpStatusShowSas(callId, "", true);
            }
        }
    }
    else if (btn == "btZrtpReset") {
        qDebug() << "reset sas for" << callId;
        pjmedia_transport *tp = (pjmedia_transport *)pjsua_call_get_user_data(callId);
        if (tp) {
            ZrtpContext* ctx = pjmedia_transport_zrtp_getZrtpContext(tp);
            if (ctx) {
                zrtp_resetSASVerified(ctx);
                emit zrtpStatusShowSas(callId, "", false);
            }
        }
    }
    else {
        qDebug() << "This button is not supported" << btn;
    }
    if (resend && canResend) {
        ButtonClick bc = { callId, btn, false };
        btnClickStack.push(bc);
        QTimer::singleShot(1000, this, SLOT(onCallBtnClickDefered()));
    }
}

void Phone::onCallBtnClickDefered()
{
    if (btnClickStack.isEmpty()) {
        return;
    }

    ButtonClick bc(btnClickStack.pop());
    onCallBtnClick(bc.button, bc.callId, bc.resend);
}

void Phone::speakerOn()
{
    pj_status_t status;
    pjmedia_aud_dev_route route = PJMEDIA_AUD_DEV_ROUTE_LOUDSPEAKER;
    status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, false);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_snd_set_settings setting audio to loadspeaker", status);
    }
}

void Phone::speakerOff()
{
    pj_status_t status;
    pjmedia_aud_dev_route route =  PJMEDIA_AUD_DEV_ROUTE_DEFAULT;
    status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, false);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_snd_set_settings setting audio to default", status);
    }
}

void Phone::onCallTransferBtnClick(const QString &btn, int callId,
                                   const QString &extension)
{
    qDebug() << "onCallTransferBtnClick" << btn << callId << extension;

    if (!sipOn) {
        logStatusMsg(tr("Phone is offline"), 10000);
        return;
    }

    if (btn == "btAmCancelXfer") {
        transferCancel(callId);
        logStatusMsg(tr("Transfer canceled"), 5000);
        return;
    }

    if (extension.isEmpty()) {
        qDebug() << "Empty extension. Cancel transfer";
        transferCancel(callId);
        logStatusMsg(tr("Please specify an extension. Transfer canceled."), 5000);
        return;
    }

    if (btn == "btAmBlndXfer") {
        qDebug() << "Blind transfer";
        blndXfer(callId, extension);
    }
    else if (btn == "btAmAttnXfer") {
        qDebug() << "Attn transfer. Handle in UI";
    }
}

void Phone::blndXfer(int callId, const QString &extension)
{
    pj_status_t status;
    const Account *ac = settingsAccounts->getPjSipAccount(app_config.call_data[callId].call_info.acc_id);
    if (!ac) {
        logMsg("blndXfer. Account not found");
        transferCancel(callId);
        return;
    }

    QString trUri("sip:" + extension + "@" + ac->domain);
    QByteArray baTrUri(trUri.toLocal8Bit());

    qDebug() << "Calling pjsip transfer for" << trUri;

    pj_str_t trDest = pj_str(baTrUri.data());
    status = pjsua_call_xfer(callId, &trDest, 0);
    if (status != PJ_SUCCESS) {
        logStatusMsg(tr("Transfer error"), 5000);
        logErrMsg("Error pjsua_call_xfer", status);
        transferCancel(callId);
    }
    qDebug() << "pjsua_call_xfer status" << status;
}

void Phone::attnXfer(int srcCallId, int destCallId)
{
    pj_status_t status;
    qDebug() << "attn transfer" << srcCallId  << destCallId;

    status =  pjsua_call_xfer_replaces(srcCallId, destCallId,
                                       PJSUA_XFER_NO_REQUIRE_REPLACES, 0);
    if (status != PJ_SUCCESS) {
        logStatusMsg(tr("Transfer error"), 5000);
        logErrMsg("Error pjsua_call_xfer_replaces", status);
        transferCancel(srcCallId);
    }
    qDebug() << "pjsua_call_xfer_replaces status" << status;
}

void Phone::unHold(int callId)
{
    if (app_config.call_data[callId].call_info.media_status == PJSUA_CALL_MEDIA_LOCAL_HOLD)
    {
        qDebug() << "Unhold call";
        pj_status_t status = pjsua_call_reinvite(callId, PJSUA_CALL_UNHOLD, 0);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error pjsua_call_reinvite", status);
        }
    }
}

void Phone::transferCancel(int callId)
{
    qDebug() << "transferCancel";
    pj_status_t status = pjsua_call_get_info(callId, &app_config.call_data[callId].call_info);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_call_get_info", status);
        return;
    }
    unHold(callId);
}

void Phone::onCallState(CallInfo ci)
{
    int call_id = ci.c_id;
    QString state(ci.state_text);
    QString last_status(ci.last_status);
    QString last_status_text(ci.last_status_text);
    bool ring = ci.ring;

    if (state != "DISCONNCTD") {
        pjsua_call_get_info(call_id, &app_config.call_data[call_id].call_info);
    }

    const Account *acnt = settingsAccounts->getPjSipAccount(ci.acc_id);
    if (!acnt) {
        logMsg("onCallState. Account not found");
        return;
    }

    if (state == "INCOMING") {
        pj_bzero(&app_config.call_data[call_id], sizeof(call_data));

        startNotify(call_id, *acnt);

        pjsua_call_answer(call_id, 180, NULL, NULL);
        ring = true;

        if (acnt->callRecAll) {
            app_config.call_data[call_id].needsRecording = true;
            app_config.call_data[call_id].deferedRecording = true;
        }
    }
    else if (state == "DISCONNCTD") {
        stopNotify(call_id);
        stopRecording(call_id);

        if (last_status != "200") {
            logStatusMsg(tr("Call ended: ") + last_status_text, 10000);
        }
        else {
            logStatusMsg(tr("Call ended"), 5000);
        }

        pj_bzero(&app_config.call_data[call_id], sizeof(call_data));
    }
    else if (state == "EARLY" && ring) {
        ringback_start(call_id);
    }
    else if (state == "CALLING") {
        if (app_config.call_data[call_id].isTransferor) {
            ci.transfereeCallId = app_config.call_data[call_id].transfereeCallId;
        }

        if (acnt->callRecAll) {
            app_config.call_data[call_id].needsRecording = true;
            app_config.call_data[call_id].deferedRecording = true;
        }
    }
    else if (state == "CONFIRMED") {
        logStatusMsg(tr("Answered"), 5000);
        if (strlen(app_config.call_data[call_id].secStagePhoneNumber)) {
            pj_str_t pjDtmf = pj_str(app_config.call_data[call_id].secStagePhoneNumber);
            pjsua_call_dial_dtmf(call_id, &pjDtmf);
        }
    }
    emit callState(ci);
}

void Phone::onMediaState(int call_id, int state)
{
    ringStop(call_id);

    if (state == PJSUA_CALL_MEDIA_ACTIVE) {
        if (app_config.call_data[call_id].deferedMute) {
            app_config.call_data[call_id].deferedMute = false;
            ButtonClick bc = { call_id, "btMute", true };
            btnClickStack.push(bc);
            QTimer::singleShot(0, this, SLOT(onCallBtnClickDefered()));
        }
        if (app_config.call_data[call_id].deferedSpeaker) {
            app_config.call_data[call_id].deferedSpeaker = false;
            ButtonClick bc = { call_id, "btSpeaker", true };
            btnClickStack.push(bc);
            QTimer::singleShot(0, this, SLOT(onCallBtnClickDefered()));
        }
        if (app_config.call_data[call_id].deferedRecording) {
            app_config.call_data[call_id].deferedRecording = false;
            ButtonClick bc = { call_id, "btRecord", true };
            btnClickStack.push(bc);
            QTimer::singleShot(0, this, SLOT(onCallBtnClickDefered()));
        }
    }

    emit mediaState(call_id, state);
}

void Phone::onRegState(const RegInfo &ri)
{
    pj_status_t status;
    pjsua_acc_info info;
    int acc_id_cb = ri.accId;

    if (pjsua_acc_is_valid(acc_id_cb)) {
        status = pjsua_acc_get_info(acc_id_cb, &info);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error getting account info", status);
            return;
        }
        logMsg("onRegState: status=" + QString::number(info.status));
        logMsg("onRegState: statustext=" + QString::fromAscii(info.status_text.ptr,info.status_text.slen));

        Account *acnt = settingsAccounts->getPjSipAccount(acc_id_cb);
        if (acnt) {
            if (info.status ==  PJSIP_SC_OK) {
                acnt->regStatus = "Registered";
                acnt->regStatusText = "";
            }
            else if (info.status == 0) {
            }
            else if (info.status == PJSIP_SC_TRYING) {
                acnt->regStatus = "Registration in progress";
                acnt->regStatusText = QString::fromAscii(info.status_text.ptr,info.status_text.slen);
            }
            else {
                acnt->regStatus = "Registration error";
                acnt->regStatusText = QString::fromAscii(info.status_text.ptr,info.status_text.slen);
            }
            acnt->regInfo = ri;

            QHashIterator<QString, TransportInfo *> tri(transports);
            while (tri.hasNext()) {
                tri.next();
                if (tri.key() == ri.transportObjName) {
                    acnt->trInfo = *tri.value();
                }
            }

            logStatusMsg(tr("Account: ") + acnt->name() + " " + acnt->infoRegistration());
            emit regState(*acnt);
        }
    }
    else {
        logMsg("onRegState: invalid acc_id");
    }
}

void Phone::onTransportState(const TransportInfo &ti)
{
    TransportInfo *trInfo;
    if (ti.state == PJSIP_TP_STATE_CONNECTED) {
        if (transports.contains(ti.objName)) {
            trInfo = transports.value(ti.objName);
        }
        else {
            trInfo = new TransportInfo;
        }
        *trInfo = ti;
        transports.insert(ti.objName, trInfo);
    }
    else {
        trInfo = transports.take(ti.objName);
        if (trInfo) {
            delete trInfo;
        }
    }

    Account *acnt;
    int i = 0;
    while ((acnt = settingsAccounts->getAccount(i))) {
        if (acnt->regInfo.transportObjName == ti.objName) {
            acnt->trInfo = ti;
            emit regState(*acnt);
            break;
        }
        ++i;
    }
}

void Phone::onCallTransferStatus(int callId, int statusCode, const QString &statusText, bool final)
{
    pj_status_t status;
    qDebug() << "onCallTransferStatus" << callId << statusCode << statusText << final;
    if (statusCode ==  PJSIP_SC_OK) {
        status = pjsua_call_hangup(callId, 0, 0, 0);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error in pjsua_call_hangup", status);
        }
        logStatusMsg(tr("Transfer completed"), 5000);
        return;
    }
    if (final) {
        logStatusMsg(tr("Unable to transfer: ") + QString(statusText), 5000);
        transferCancel(callId);
    }
}

void Phone::onStartWavPlayback(const QString &wavFile)
{
    if (wavFile.isEmpty())
        return;

    onStopWavPlayback();

    QByteArray baFn(wavFile.toLocal8Bit());
    pj_str_t filename = pj_str(baFn.data());

    pj_status_t status = pjsua_player_create(&filename, PJMEDIA_FILE_NO_LOOP, &p_id);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_player_create", status);
    }
    player_port =  pjsua_player_get_conf_port(p_id);

    status = pjsua_conf_connect(player_port, 0);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_conf_connect playerport -> 0", status);
    }
}

void Phone::onStopWavPlayback()
{
    if (!player_port)
        return;

    pj_status_t status = pjsua_conf_disconnect(player_port, 0);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_conf_disconnect playerport -> 0", status);
    }
    status =  pjsua_player_destroy(p_id);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_player_destroy", status);
    }

    player_port = 0;
    p_id = 0;
}

void Phone::setSelectedCall(int callId)
{
    for (int i = 0; i < PJSUA_MAX_CALLS; ++i)
        app_config.call_data[i].isSelected = false;
    app_config.call_data[callId].isSelected = true;
}


int Phone::getNumberOfCalls() const
{
    int numCalls = 0;
    for (int i = 0; i < PJSUA_MAX_CALLS; ++i) {
        if (app_config.call_data[i].call_info.conf_slot) {
            numCalls++;
        }
    }
    return numCalls;
}

void Phone::ringback_start(pjsua_call_id call_id)
{
    if (app_config.no_tones)
        return;

    if (app_config.call_data[call_id].ringback_on)
        return;

    app_config.call_data[call_id].ringback_on = PJ_TRUE;

    if (++app_config.ringback_cnt == 1 &&
            app_config.ringback_slot != PJSUA_INVALID_ID)
    {
        pjsua_conf_connect(app_config.ringback_slot, 0);

        if (app_config.call_data[call_id].deferedSpeaker) {
            speakerOn();
        }
    }
}

void Phone::ringStop(pjsua_call_id call_id)
{
    pj_status_t status;

    if (app_config.no_tones)
        return;

#ifdef BB10_BLD
    pjmedia_aud_dev_route route =  PJMEDIA_AUD_DEV_ROUTE_DEFAULT;
    status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, true);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_snd_set_settings setting audio to default", status);
    }
#endif

    if (app_config.call_data[call_id].ringback_on) {
        app_config.call_data[call_id].ringback_on = PJ_FALSE;

        pj_assert(app_config.ringback_cnt > 0);
        if (--app_config.ringback_cnt == 0 &&
                app_config.ringback_slot != PJSUA_INVALID_ID)
        {
            pjsua_conf_disconnect(app_config.ringback_slot, 0);
            pjmedia_tonegen_rewind(app_config.ringback_port);
        }
    }

    if (app_config.call_data[call_id].ring_on) {
        app_config.call_data[call_id].ring_on = PJ_FALSE;

        if (app_config.call_data[call_id].ring_player_port) {
            status = pjsua_conf_disconnect(
                        app_config.call_data[call_id].ring_player_port, 0);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_disconnect ring_player_port->0", status);
            }
            status =  pjsua_player_destroy(
                        app_config.call_data[call_id].ring_player);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_player_destroy", status);
            }
            app_config.call_data[call_id].ring_player_port = 0;
            app_config.call_data[call_id].ring_player = 0;
        }
        else {
            pj_assert(app_config.ring_cnt > 0);
            if (--app_config.ring_cnt == 0 &&
                    app_config.ring_slot != PJSUA_INVALID_ID)
            {
                qDebug() << "ringStop disconected tone based ringer";
                pjsua_conf_disconnect(app_config.ring_slot, 0);
                pjmedia_tonegen_rewind(app_config.ring_port);
            }
        }
    }
}

void Phone::ringStart(pjsua_call_id call_id, int ringToneIndex)
{
    pj_status_t status;

    if (app_config.no_tones)
        return;

    if (app_config.call_data[call_id].ring_on) {
        return;
    }
    else {
        app_config.call_data[call_id].ring_on = PJ_TRUE;
    }

    if (ringToneIndex) {
        QString ringPath("");
#if defined(Q_OS_QNX)
        ringPath.append("app/native/");
#endif
        ringPath.append("sounds/ring" + QString::number(ringToneIndex) + ".wav");
        QByteArray baFn(ringPath.toLocal8Bit());
        pj_str_t wavFile = pj_str(baFn.data());
        pjsua_player_id player_id;
        status = pjsua_player_create(&wavFile, 0, &player_id);
        if (status != PJ_SUCCESS)
            return;
        app_config.call_data[call_id].ring_player = player_id;
        pjsua_conf_port_id rp = pjsua_player_get_conf_port(player_id);
        app_config.call_data[call_id].ring_player_port = rp;
        pjsua_conf_connect(rp, 0);
    }
    else {
        if (++app_config.ring_cnt == 1 &&
                app_config.ring_slot != PJSUA_INVALID_ID)
        {
            pjsua_conf_port_id rp = app_config.ring_slot;
            pjsua_conf_connect(rp, 0);
        }
        else {
        }
    }

#ifdef BB10_BLD
    if (!getNumberOfCalls()) {
        pjmedia_aud_dev_route route = PJMEDIA_AUD_DEV_ROUTE_LOUDSPEAKER;
        status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, false);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error pjsua_snd_set_settings setting audio to loadspeaker", status);
        }
    }
#endif
}

void Phone::ringCreate()
{
    pj_status_t status;

    char tone_name[] = "ringback";
    char ring_name[] = "ring";

    if (app_config.no_tones)
        return;

    app_config.tone_count = 0;

    unsigned i, samples_per_frame;
    pjmedia_tone_desc tone[RING_CNT+RINGBACK_CNT];
    pj_str_t name;

    samples_per_frame = app_config.media_cfg.audio_frame_ptime *
            app_config.media_cfg.clock_rate *
            app_config.media_cfg.channel_count / 1000;

    name = pj_str(tone_name);
    status = pjmedia_tonegen_create2(app_config.pool, &name,
                                     app_config.media_cfg.clock_rate,
                                     app_config.media_cfg.channel_count,
                                     samples_per_frame,
                                     16, PJMEDIA_TONEGEN_LOOP,
                                     &app_config.ringback_port);
    if (status != PJ_SUCCESS)
        goto on_error;

    pj_bzero(&tone, sizeof(tone));
    for (i = 0; i < RINGBACK_CNT; ++i) {
        tone[i].freq1 = RINGBACK_FREQ1;
        tone[i].freq2 = RINGBACK_FREQ2;
        tone[i].on_msec = RINGBACK_ON;
        tone[i].off_msec = RINGBACK_OFF;
    }
    tone[RINGBACK_CNT-1].off_msec = RINGBACK_INTERVAL;

    pjmedia_tonegen_play(app_config.ringback_port, RINGBACK_CNT, tone,
                         PJMEDIA_TONEGEN_LOOP);


    status = pjsua_conf_add_port(app_config.pool, app_config.ringback_port,
                                 &app_config.ringback_slot);
    if (status != PJ_SUCCESS)
        goto on_error;

    name = pj_str(ring_name);
    status = pjmedia_tonegen_create2(app_config.pool, &name,
                                     app_config.media_cfg.clock_rate,
                                     app_config.media_cfg.channel_count,
                                     samples_per_frame,
                                     16, PJMEDIA_TONEGEN_LOOP,
                                     &app_config.ring_port);
    if (status != PJ_SUCCESS)
        goto on_error;

    for (i=0; i<RING_CNT; ++i) {
        tone[i].freq1 = RING_FREQ1;
        tone[i].freq2 = RING_FREQ2;
        tone[i].on_msec = RING_ON;
        tone[i].off_msec = RING_OFF;
    }
    tone[RING_CNT-1].off_msec = RING_INTERVAL;

    pjmedia_tonegen_play(app_config.ring_port, RING_CNT,
                         tone, PJMEDIA_TONEGEN_LOOP);

    status = pjsua_conf_add_port(app_config.pool, app_config.ring_port,
                                 &app_config.ring_slot);
    if (status != PJ_SUCCESS)
        goto on_error;

    on_error:
        return;
}

void Phone::onSetLogLevel(int level)
{
    logMsg("Log level set to: " + QString::number(level));

    app_config.log_cfg.console_level = level;
    if (sipOn)
        pjsua_reconfigure_logging(&app_config.log_cfg);
}

void Phone::onStartTestRing(int ringToneIndex)
{
    pj_status_t status;

    onStopTestRing();

    if (ringToneIndex) {
        QString ringPath("");
#if defined(Q_OS_QNX)
        ringPath.append("app/native/");
#endif
        ringPath.append("sounds/ring" + QString::number(ringToneIndex) + ".wav");
        QByteArray baFn(ringPath.toLocal8Bit());
        pj_str_t wavFile = pj_str(baFn.data());
        status = pjsua_player_create(&wavFile, 0, &app_config.test_ring_player);
        if (status != PJ_SUCCESS)
            return;
        app_config.test_ring_player_port = pjsua_player_get_conf_port(app_config.test_ring_player);
        status = pjsua_conf_connect(app_config.test_ring_player_port, 0);
    }
    else {
        status = pjsua_conf_connect(app_config.ring_slot, 0);
    }
    if (status != PJ_SUCCESS)
        return;

#ifdef BB10_BLD
    pjmedia_aud_dev_route route = PJMEDIA_AUD_DEV_ROUTE_LOUDSPEAKER;
    status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, false);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_snd_set_settings setting audio to loadspeaker", status);
    }
#endif

    m_ringTestTimer->start();
}

void Phone::onStopTestRing()
{
    pj_status_t status;

    m_ringTestTimer->stop();

#ifdef BB10_BLD
    pjmedia_aud_dev_route route =  PJMEDIA_AUD_DEV_ROUTE_DEFAULT;
    status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, true);
    if (status != PJ_SUCCESS) {
        logErrMsg("Error pjsua_snd_set_settings setting audio to default", status);
    }
#endif

    if (app_config.test_ring_player_port) {
        status = pjsua_conf_disconnect(app_config.test_ring_player_port, 0);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error pjsua_conf_disconnect test_ring_player_port->0", status);
        }
        app_config.test_ring_player_port = 0;
    }
    if (app_config.test_ring_player) {
        status =  pjsua_player_destroy(app_config.test_ring_player);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error test_ring_player pjsua_player_destroy", status);
        }
        app_config.test_ring_player = 0;
    }
    if (app_config.ring_slot != PJSUA_INVALID_ID) {
        status = pjsua_conf_disconnect(app_config.ring_slot, 0);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error pjsua_conf_disconnect ring_slot->0", status);
        }
        pjmedia_tonegen_rewind(app_config.ring_port);
    }
}

void Phone::onSetConfConnect(int call_id, bool state)
{
    pj_status_t status;
    pjsua_conf_port_id sl1, sl2, rec_port;

    app_config.call_data[call_id].isInConf = state;

    qDebug() << "onSetConfConnect call:" << call_id << state;

    sl2 =      app_config.call_data[call_id].call_info.conf_slot;
    rec_port = app_config.call_data[call_id].rec_conf_port;

    if (!(sl2 > 0)) {
        logMsg("sl2 is not set");
        return;
    }

    for (int i = 0; i < PJSUA_MAX_CALLS; ++i) {
        if (!app_config.call_data[i].isInConf)
            continue;
        if (i == call_id)
            continue;
        sl1 = app_config.call_data[i].call_info.conf_slot;
        if (!(sl1 > 0))
            continue;

        if (state) {
            qDebug() << "onSetConfConnect connect calls"
                     << call_id << "and" << i << "slots" << sl2 << "and" << sl1;

            status = pjsua_conf_connect(sl1, sl2);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_connect", status);
            }
            status = pjsua_conf_connect(sl2, sl1);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_connect", status);
            }
            if (app_config.call_data[call_id].isRecorded) {
                status = pjsua_conf_connect(sl1, rec_port);
                if (status != PJ_SUCCESS) {
                    logErrMsg("Error pjsua_conf_connect recport", status);
                }
            }
        }
        else {
            qDebug() << "onSetConfConnect disconnect calls"
                     << call_id << "and" << i << sl2 << "and" << sl1;
            status = pjsua_conf_disconnect(sl1, sl2);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_disconnect", status);
            }
            status = pjsua_conf_disconnect(sl2, sl1);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_disconnect", status);
            }
            if (app_config.call_data[call_id].isRecorded) {
                status = pjsua_conf_disconnect(sl1, rec_port);
                if (status != PJ_SUCCESS) {
                    logErrMsg("Error pjsua_conf_disconnect recport", status);
                }
            }
        }
    }
}

void Phone::onConfButtonClicked(const QString &btnName)
{
    if (btnName == "btConfRec") {
        confIsRec = !confIsRec;
        setConfRec(confIsRec);
    }
    if (btnName == "btConfSpeaker") {
        confIsOnSpeaker = !confIsOnSpeaker;

        if (confIsOnSpeaker) {
            pjmedia_aud_dev_route route = PJMEDIA_AUD_DEV_ROUTE_LOUDSPEAKER;
            pj_status_t status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, false);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_snd_set_settings setting audio to loadspeaker", status);
            }
        }
        else {
            pjmedia_aud_dev_route route =  PJMEDIA_AUD_DEV_ROUTE_DEFAULT;
            pj_status_t status = pjsua_snd_set_setting(PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE, &route, false);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_snd_set_settings setting audio to default", status);
            }
        }
    }
    else if (btnName == "btConfSplit") {
    }
    else if (btnName == "btConfMute") {
        confIsMute = !confIsMute;
        setConfMute(-1, confIsMute);
    }
    else if (btnName == "btConfHangup") {
    }
}

void Phone::setConfRec(bool state)
{
    for (int i = 0; i < PJSUA_MAX_CALLS; ++i) {
        if (!app_config.call_data[i].isInConf)
            continue;

        setRecording(i, state);
        setConfRecording(i, state);
    }
}

void Phone::setRecording(int call_id, bool state)
{
    pj_status_t status;

    qDebug() << "Toggle recording for call" << call_id;

    if (!app_config.call_data[call_id].rec_conf_port) {
        qDebug() << "Current dir" << QCoreApplication::applicationDirPath();
#if defined(Q_OS_QNX)
        QDir dir("shared/voice/callrecording");
#else
        QDir dir("callrecording");
#endif
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QDateTime now(QDateTime::currentDateTime());
        QString fn(now.toString("yyyyMMdd-hhmmss-zzz"));
        fn.append(".wav");
        fn.prepend("/");
        fn.prepend(dir.path());

        QByteArray baFn(fn.toLocal8Bit());
        pj_str_t filename = pj_str(baFn.data());

        pjsua_recorder_id rec_id;
        status = pjsua_recorder_create(&filename, 0, NULL, -1, 0, &rec_id);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error in pjsua_recorder_create", status);
            return;
        }
        else {
            app_config.call_data[call_id].rec_id = rec_id;
            app_config.call_data[call_id].rec_conf_port = pjsua_recorder_get_conf_port(rec_id);
        }
        emit callRecorded(call_id, fn, true);
    }

    pjsua_conf_port_id rec_port =  app_config.call_data[call_id].rec_conf_port;
    pjsua_conf_port_id conf_slot = app_config.call_data[call_id].call_info.conf_slot;

    if (state) {
        status = pjsua_conf_connect(0, rec_port);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error in pjsua_conf_connect snd->recport", status);
        }
        if (conf_slot != PJSUA_INVALID_ID) {
            status = pjsua_conf_connect(conf_slot, rec_port);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error in pjsua_conf_connect caller->recport", status);
            }
            app_config.call_data[call_id].isRecorded = true;
        }
        else {
            logMsg("Error attempting to connect recording port to invalid conf_slot");
        }
    }
    else {
        status = pjsua_conf_disconnect(0, rec_port);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error in pjsua_conf_disconnect snd->recport", status);
        }

        status = pjsua_conf_disconnect(conf_slot, rec_port);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error in pjsua_conf_disconnect caller->recport", status);
        }
        app_config.call_data[call_id].isRecorded = false;
        emit callRecorded(call_id, "", false);
    }
}

void Phone::stopRecording(int call_id)
{
    pj_status_t status;
    if (app_config.call_data[call_id].rec_conf_port) {
        status = pjsua_recorder_destroy(app_config.call_data[call_id].rec_id);
        if (status != PJ_SUCCESS) {
            logErrMsg("Error in pjsua_recorder_destroy", status);
        }
    }
}

void Phone::setConfRecording(int call_id, bool state)
{
    pj_status_t status;
    pjsua_conf_port_id sl1, rec_port;

    rec_port = app_config.call_data[call_id].rec_conf_port;

    for (int i = 0; i < PJSUA_MAX_CALLS; ++i) {
        if (!app_config.call_data[i].isInConf)
            continue;
        if (i == call_id)
            continue;
        sl1 = app_config.call_data[i].call_info.conf_slot;
        if (!(sl1 > 0))
            continue;

        if (state) {
            status = pjsua_conf_connect(sl1, rec_port);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_connect rec_port", status);
            }
        }
        else {
            status = pjsua_conf_disconnect(sl1, rec_port);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_disconnect rec_port", status);
            }
        }
    }
}

void Phone::setConfMute(int call_id, bool state)
{
    pj_status_t status;
    pjsua_conf_port_id call_mic, sl1, rec_port;
    bool isRec;

    if (call_id == -1)
        call_mic = 0;
    else {
        call_mic = app_config.call_data[call_id].call_info.conf_slot;
        if (state) {
            status = pjsua_conf_disconnect(call_mic, 0);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_disconnect mute", status);
            }
        }
        else {
            status = pjsua_conf_connect(call_mic, 0);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_connect mute", status);
            }
        }
    }

    for (int i = 0; i < PJSUA_MAX_CALLS; ++i) {
        if (!app_config.call_data[i].isInConf)
            continue;
        if (i == call_id)
            continue;

        sl1 = app_config.call_data[i].call_info.conf_slot;
        rec_port = app_config.call_data[i].rec_conf_port;
        isRec = app_config.call_data[i].isRecorded;

        if (state) {
            status = pjsua_conf_disconnect(call_mic, sl1);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_disconnect mute", status);
            }
            if (isRec) {
                status = pjsua_conf_disconnect(call_mic, rec_port);
                if (status != PJ_SUCCESS) {
                    logErrMsg("Error pjsua_conf_disconnect mute rec", status);
                }
            }
        }
        else {
            status = pjsua_conf_connect(call_mic, sl1);
            if (status != PJ_SUCCESS) {
                logErrMsg("Error pjsua_conf_connect mute", status);
            }
            if (isRec) {
                status = pjsua_conf_connect(call_mic, rec_port);
                if (status != PJ_SUCCESS) {
                    logErrMsg("Error pjsua_conf_connect mute rec", status);
                }
            }
        }
    }
}

void Phone::startNotify(int call_id, const Account &account)
{
#ifdef BB10_BLD
    NotificationGlobalSettings globalSettings;
    NotificationApplicationSettings appSettings;
    switch(globalSettings.led()) {
    case NotificationPolicy::Allow:
        if (appSettings.alertsEnabled() == false ||
            appSettings.led() == NotificationPolicy::Deny)
        {
          notifyLed = false;
        }
        else {
            notifyLed = true;
        }
        break;
    case NotificationPolicy::Deny:
        notifyLed = false;
        break;
    default:
        notifyLed = false;
        break;
    }
    switch(globalSettings.sound()) {
    case NotificationPolicy::Allow:
        if (appSettings.alertsEnabled() == false ||
            appSettings.sound() == NotificationPolicy::Deny)
        {
            notifySound = false;
        }
        else {
            notifySound = true;
        }
        break;
    case NotificationPolicy::Deny:
        notifySound = false;
        break;
    default:
        notifySound = true;
        break;
    }
    switch(globalSettings.vibrate()) {
    case NotificationPolicy::Allow:
        if (appSettings.alertsEnabled() == false ||
            appSettings.vibrate() == NotificationPolicy::Deny)
        {
            notifyVibrate = false;
        }
        else {
            notifyVibrate = true;
        }
        break;
    case NotificationPolicy::Deny:
        notifyVibrate = false;
        break;
    default:
        notifyVibrate = false;
        break;
    }
#endif

    if (notifySound) {
        int ringToneIndex = 0;
        if (account.ringTone) {
            ringToneIndex = account.ringTone;
        }
        ringStart(call_id, ringToneIndex);
    }
#ifdef BB10_BLD
    if (notifyVibrate) {
        vibration_request(VIBRATION_INTENSITY_MEDIUM, 2000);
    }
    if (notifyLed) {
        QString callStr = QString::number(call_id);
        led_request_color(callStr.toLatin1().constData(), LED_COLOR_RED, 5);
    }
#endif
}

void Phone::stopNotify(int call_id)
{
    if (notifySound) {
        ringStop(call_id);
    }
#ifdef BB10_BLD
    if (notifyVibrate) {
        vibration_request(VIBRATION_INTENSITY_MEDIUM, 0);
    }
    if (notifyLed) {
        QString callStr = QString::number(call_id);
        led_cancel(callStr.toLatin1().constData());
    }
#endif
}

void Phone::onMsgNew(MessageRec *msg)
{
    if (msg->direction == MessageRec::Out) {
        pj_status_t status;
        pj_str_t pjto, pjtext;

        QString toStr(msg->to.getDirectCallUri());
        QByteArray tempto(toStr.toLocal8Bit());
        QByteArray temptext(msg->text.toLocal8Bit());
        pjto = pj_str(tempto.data());
        pjtext = pj_str(temptext.data());

        status = pjsua_im_send(msg->pjSipAccId, &pjto,
                               NULL, &pjtext,
                               NULL, &msg->id);
        if (status != PJ_SUCCESS) {
            logErrMsg(tr("Error sending IM"), status);
            msg->deliveryStatus = MessageRec::Failed;
        }
        else {
            msg->deliveryStatus = MessageRec::Queued;
        }
    }
    else {
    }
    emit msgNew(msg);
}

void Phone::onMsgStatus(const MessageRec &msg)
{
    emit msgStatus(msg);
}

void Phone::onMwiState(int acc_id, bool mwi, int numNewMsg, int numOldMsg)
{
    if (pjsua_acc_is_valid(acc_id)) {
        Account *acnt = settingsAccounts->getPjSipAccount(acc_id);
        if (acnt) {
            acnt->mwiStatus = mwi;
            acnt->mwiNumNewMsg = numNewMsg;
            acnt->mwiNumOldMsg = numOldMsg;
            emit regState(*acnt);
        }
    }
    else {
        logMsg("onMwiState: invalid acc_id");
    }
}

void Phone::onZrtpStatusSecureOn(int call_id, const QString &cipher)
{
    emit zrtpStatusSecureOn(call_id, cipher);
}

void Phone::onZrtpStatusSecureOff(int call_id)
{
    emit zrtpStatusSecureOff(call_id);
}

void Phone::onZrtpStatusShowSas(int call_id, const QString &sas, int verified)
{
    emit zrtpStatusShowSas(call_id, sas, verified);
}
