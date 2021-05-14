/* $Id: phone.h */
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

#ifndef PHONE_H
#define PHONE_H

#include "account.h"
#include "accountssettings.h"
#include "generalsettings.h"
#include "cont.h"
#include "messagerec.h"
#include "reginfo.h"
#include "transportinfo.h"
#include "PjCallback.h"
#include <pjsua-lib/pjsua.h>
#include <QObject>
#include <QMutex>
#include <QPointer>
#include <QStack>
#include <QHash>
#include <QTemporaryFile>

class QTimer;

/* Call specific data */
struct call_data
{
    pj_timer_entry  timer;
    pj_bool_t       ringback_on;
    pj_bool_t       ring_on;
    pjsua_call_info call_info;
    pjsua_recorder_id rec_id;
    pjsua_conf_port_id rec_conf_port;
    pjsua_player_id ring_player;
    pjsua_conf_port_id ring_player_port;
    bool isSelected;
    bool isInConf;
    bool isRecorded;
    bool needsRecording;
    bool deferedRecording;
    bool isTransferor;
    int transfereeCallId;   // Transferee call id or -1 for normal calls.
    char name[100];
    bool deferedSpeaker;
    bool deferedMute;
    char secStagePhoneNumber[128];
};

#define MAX_ACCOUNTS 128

struct acc_data {
    pjsua_acc_id acc_id;
    pjsua_acc_config acc_cfg;
    Account *ac;
    //pjsua_transport_id transport_id;
    //pjsua_transport_config transport_cfg;
};

/* Global structures for pjsip config */
struct app_config
{
    pjsua_config cfg;
    pjsua_logging_config log_cfg;
    pjsua_media_config media_cfg;
    //pjsua_transport_config transport_cfg;
    //pjsua_transport_config rtp_cfg;
    //pjsua_buddy_config buddy_cfg;

    // New pool of accounts
    struct acc_data accounts[MAX_ACCOUNTS];

    // Transports
    pjsua_transport_config udp_cfg;
    pjsua_transport_id udp_transport_id;
    pjsua_transport_config tcp_cfg;
    pjsua_transport_id tcp_transport_id;
    pjsua_transport_config tls_cfg;
    pjsua_transport_id tls_transport_id;

    pj_pool_t *pool;
    struct call_data        call_data[PJSUA_MAX_CALLS];

    unsigned                tone_count;
    pjmedia_tone_desc       tones[32];
    pjsua_conf_port_id      tone_slots[32];

    pj_bool_t               no_tones;
    int                     ringback_slot;
    int                     ringback_cnt;
    pjmedia_port           *ringback_port;
    int                     ring_slot;
    int                     ring_cnt;
    pjmedia_port           *ring_port;
    pjsua_player_id         test_ring_player;
    pjsua_conf_port_id      test_ring_player_port;
    pjmedia_conf           *conf;
};

struct ButtonClick {
    int callId;
    QString button;
    bool resend;
};

class Phone : public QObject
{
    Q_OBJECT

public:
    explicit Phone(QObject *parent = 0);
    ~Phone();

private:
    QPointer<PjCallback> pjCallback;
    GeneralSettings *settingsGeneral;
    AccountsSettings *settingsAccounts;
    bool sipOn;
    bool confIsRec;
    bool confIsMute;
    bool confIsOnSpeaker;
    bool netAvailable;

    pj_thread_desc phThrDesc;
    pj_thread_t *phThrHandle;

    pjsua_player_id p_id;
    pjsua_conf_port_id player_port;
    bool isSpeaker;
    QString appVersion;

    struct app_config app_config;
    QMutex configReload;
    QTimer *m_ringTestTimer;
    QStack<ButtonClick> btnClickStack;

    bool m_init;

    bool notifyLed;
    bool notifySound;
    bool notifyVibrate;

    QHash<QString, TransportInfo *> transports;

    QTemporaryFile rootCAcert;

    void init();
    int initPjSipAccount(int, Account *ac);
    int initPjSip();
    int shutdownPjSip();

    pj_status_t activeCall(pjsua_call_id *current_call);

    //int getSelectedCall();

    void setConfRec(bool state);
    void setRecording(int call_id, bool state);
    void stopRecording(int call_id);
    void setConfRecording(int call_id, bool state);
    void setConfMute(int call_id, bool state);

    void startNotify(int call_id, const Account &account);
    void stopNotify(int call_id);

    int getNumberOfCalls() const;

    void ringback_start(pjsua_call_id call_id);
    void ringStop(pjsua_call_id call_id);
    void ringStart(pjsua_call_id call_id, int ringToneIndex);
    void ringCreate();

    void unHold(int callId);
    void transferCancel(int callId);
    void blndXfer(int callId, const QString &extension);
    void attnXfer(int srcCallId, int destCallId);

    void speakerOn();
    void speakerOff();

public slots:
    void onReloadPhone(bool newNetStatus);
    void onSendDTMF(int callId, const QString &key);
    void onCallContact(const Cont &cont, int transfereeCallId = -1);
    void onCallBtnClick(const QString &btn, int callId, bool canResend);
    void onCallTransferBtnClick(const QString &btn, int callId, const QString &extension);

    void onCallState(CallInfo ci);
    void onMediaState(int call_id, int state);
    void onRegState(const RegInfo &ri);
    void onTransportState(const TransportInfo &ti);
    void onCallTransferStatus(int callId, int statusCode, const QString &statusText, bool final);

    void onStartWavPlayback(const QString &wavFile);
    void onStopWavPlayback();

    void onSetLogLevel(int level);

    void onStartTestRing(int ringToneIndex);
    void onStopTestRing();

    void onSetConfConnect(int call_id, bool state);
    void onConfButtonClicked(const QString &btnName);

    void setSelectedCall(int callId);
    void logMsg(const QString &logText);
    void logErrMsg(const QString &logText, pj_status_t status);
    void logStatusMsg(const QString &logText, int timeOut = 0);
    void logStatusErrMsg(const QString &logText, pj_status_t status, int timeOut = 0);

    void onMsgNew(MessageRec *msg);
    void onMsgStatus(const MessageRec &msg);

    void onMwiState(int acc_id, bool mwi, int numNewMsg, int numOldMsg);

    void onZrtpStatusSecureOn(int call_id, const QString &cipher);
    void onZrtpStatusSecureOff(int call_id);
    void onZrtpStatusShowSas(int call_id, const QString &sas, int verified);

private slots:
    void startPhone();
    void onCallBtnClickDefered();

signals:
    void logInfo(const QString &logText);
    void logStatus(const QString &logText, int timeOut=0);

    void setPjCallState(int call_id, const QString &state, const QString &last_status,
                        const QString &last_status_text, bool ring);
    void setPjMediaState(int call_id, int state);
    void setPjRegState(int acc_id, int status);

    void notifyBegin(int call_id, const Account &account);
    void notifyEnd(int call_id);

    void callRecorded(int call_id, const QString &fn, bool rec);
    void callState(const CallInfo &ci);
    void mediaState(int call_id, int state);
    void regState(const Account &);

    void getTransfereeNumber(int call_id);

    void msgStatus(const MessageRec &);
    void msgNew(MessageRec *);

    void zrtpStatusSecureOn(int call_id, const QString &cipher);
    void zrtpStatusSecureOff(int call_id);
    void zrtpStatusShowSas(int call_id, const QString &sas, int verified);
};

#endif // PHONE_H
