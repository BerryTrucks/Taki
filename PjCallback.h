/* $Id: PjCallback.h */
/*
 * Copyright (C) 2012 ipcom
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

#ifndef PJCALLBACK_H_
#define PJCALLBACK_H_

#include "callinfo.h"
#include "messagerec.h"
#include "reginfo.h"
#include "transportinfo.h"
#include <transport_zrtp.h>
#include <pjsua-lib/pjsua.h>
#include <QObject>


class PjCallback : public QObject {
    Q_OBJECT

public:
    PjCallback();
    virtual ~PjCallback();

    /* callback logger function which emmits the signal */
    void logger_cb(int level, const char *data, int len);
    /* callback logger function called by pjsip */
    static void logger_cb_wrapper(int level, const char *data, int len);

    /* callback function, called by wrapper */
    void on_pager(pjsua_call_id call_id, const pj_str_t *from,
                  const pj_str_t *to, const pj_str_t *contact,
                  const pj_str_t *mime_type, const pj_str_t *text);
    /* callback wrapper function called by pjsip
     * Incoming IM message (i.e. MESSAGE request)!*/
    static void on_pager_wrapper(pjsua_call_id call_id, const pj_str_t *from,
                                 const pj_str_t *to, const pj_str_t *contact,
                                 const pj_str_t *mime_type, const pj_str_t *text);

    /* Notify application when invite state has changed. Application may
     * then query the call info to get the detail call states by calling
     * pjsua_call_get_info() function.callback wrapper function called by pjsip */
    void on_call_state(pjsua_call_id call_id, pjsip_event *e);
    static void on_call_state_wrapper(pjsua_call_id call_id, pjsip_event *e);

    /* Notify application on incoming call. */
    void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
    static void on_incoming_call_wrapper(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);

    /* Notify application which NAT type was detected */
    void on_nat_detect(const pj_stun_nat_detect_result *res);
    static void on_nat_detect_wrapper(const pj_stun_nat_detect_result *res);

    /* Notify application when media state in the call has changed. Normal
     * application would need to implement this callback, e.g. to connect
     * the call's media to sound device. When ICE is used, this callback
     * will also be called to report ICE negotiation failure. */
    void on_call_media_state(pjsua_call_id call_id);
    static void on_call_media_state_wrapper(pjsua_call_id call_id);

    /* callback function, called by wrapper */
    void on_buddy_state(pjsua_buddy_id buddy_id);
    /* callback wrapper function called by pjsip
     * Presence state of buddy was changed */
    static void on_buddy_state_wrapper(pjsua_buddy_id buddy_id);

    /* callback function, called by wrapper */
    void on_reg_state2(pjsua_acc_id acc_id, pjsua_reg_info *info);
    /* callback wrapper function called by pjsip
     * Registration state of account changed */
    static void on_reg_state_wrapper2(pjsua_acc_id acc_id, pjsua_reg_info *info);

    void on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info);
    static void on_mwi_info_wrapper(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info);

    pj_bool_t default_mod_on_rx_request(pjsip_rx_data *rdata);
    static pj_bool_t default_mod_on_rx_request_wrapper(pjsip_rx_data *rdata);

    /* Notify application of the status of previously sent call transfer request. */
    void on_call_transfer_status(pjsua_call_id call_id, int st_code,
                                 const pj_str_t *st_text, pj_bool_t final,
                                 pj_bool_t *p_cont);
    static void on_call_transfer_status_wrapper(pjsua_call_id call_id, int st_code,
                                         const pj_str_t *st_text, pj_bool_t final,
                                         pj_bool_t *p_cont);

    /* General notification callback which is called whenever a transaction
     * within the call has changed state. */
    void on_call_tsx_state(pjsua_call_id call_id, pjsip_transaction *tsx, pjsip_event *e);
    static void on_call_tsx_state_wrapper(pjsua_call_id call_id, pjsip_transaction *tsx,
                                          pjsip_event *e);

    /* Incoming IM message (i.e. MESSAGE request)!*/
    void on_pager2(pjsua_call_id call_id, const pj_str_t *from,
                   const pj_str_t *to, const pj_str_t *contact,
                   const pj_str_t *mime_type, const pj_str_t *text,
                   pjsip_rx_data *rdata, pjsua_acc_id acc_id);
    static void on_pager2_wrapper(pjsua_call_id call_id, const pj_str_t *from,
                                  const pj_str_t *to, const pj_str_t *contact,
                                  const pj_str_t *mime_type, const pj_str_t *text,
                                  pjsip_rx_data *rdata, pjsua_acc_id acc_id);

    /* IM message status */
    void on_pager_status2(pjsua_call_id call_id, const pj_str_t *to,
                          const pj_str_t *body, void *user_data,
                          pjsip_status_code status, const pj_str_t *reason,
                          pjsip_tx_data *tdata, pjsip_rx_data *rdata,
                          pjsua_acc_id acc_id);
    static void on_pager_status2_wrapper(pjsua_call_id call_id, const pj_str_t *to,
                                         const pj_str_t *body, void *user_data,
                                         pjsip_status_code status, const pj_str_t *reason,
                                         pjsip_tx_data *tdata, pjsip_rx_data *rdata,
                                         pjsua_acc_id acc_id);

    /* IM typing notification */
    void on_typing2(pjsua_call_id call_id, const pj_str_t *from,
                    const pj_str_t *to, const pj_str_t *contact,
                    pj_bool_t is_typing, pjsip_rx_data *rdata,
                    pjsua_acc_id acc_id);
    static void on_typing2_wrapper(pjsua_call_id call_id, const pj_str_t *from,
                                   const pj_str_t *to, const pj_str_t *contact,
                                   pj_bool_t is_typing, pjsip_rx_data *rdata,
                                   pjsua_acc_id acc_id);

    /* This callback is called when transport state is changed */
    void on_transport_state(pjsip_transport *tp,
                            pjsip_transport_state state,
                            const pjsip_transport_state_info *info);
    static void on_transport_state_wrapper(pjsip_transport *tp,
                                           pjsip_transport_state state,
                                           const pjsip_transport_state_info *info);

    static pjmedia_transport* on_create_media_transport(
            pjsua_call_id call_id,
            unsigned media_idx,
            pjmedia_transport *base_tp,
            unsigned flags);

    static zrtp_UserCallbacks * zrtpCbs[PJSUA_MAX_CALLS];

    static void secureOn_wrapper(void* data, char* cipher);
    void secureOn(void* data, char* cipher);
    static void secureOff_wrapper(void* data);
    void secureOff(void* data);
    static void showSAS_wrapper(void* data, char* sas, int32_t verified);
    void showSAS(void* data, char* sas, int32_t verified);
    static void confirmGoClear(void* data);
    static void showMessage_wrapper(void* data, int32_t sev, int32_t subCode);
    void showMessage(void* data, int32_t sev, int32_t subCode);
    static void zrtpNegotiationFailed(void* data, int32_t severity, int32_t subCode);
    static void zrtpNotSuppOther(void* data);
    static void zrtpAskEnrollment(void* data, int32_t info);
    static void zrtpInformEnrollment(void* data, int32_t info);
    static void signSAS(void* data, unsigned char* sas);
    static int32_t checkSASSignature(void* data, unsigned char* sas);

private:
    pjsua_call_id getCallId(void* data) const;

signals:
    /* this signal forwards the log message a-synchronous to the GUI thread */
    void new_log_message(const QString &text);
    /* this signal forwards the instant message a-synchronous to the GUI thread */
    void new_im(const QString &from, const QString &text);
    /* this signal forwards the instant message a-synchronous to the GUI thread */
    void nat_detect(const QString &text, const QString &description);
    /* this signal forwards the call state synchronous to the GUI thread */
    void call_state_sync(int call_id);
    /* this signal forwards the call state a-synchronous BLOCKING to the GUI thread */
    void setCallState(CallInfo);
    void setMediaState(int call_id, int media_status);
    /* this signal forwards the buddy_id of the buddy whose status changed to the GUI thread */
    void buddy_state(int buddy_id);
    /* this signal forwards the acc_id of the SIP account whose registration status changed */
    void reg_state(const RegInfo &);
    /* call transfer progress and status */
    void call_transfer_status(int, int, const QString &, bool);
    /* Incoming IM */
    void on_pager(MessageRec *);
    /* IM status */
    void on_pager_status(const MessageRec &);
    /* MWI update */
    void mwi_state(int, bool, int, int);
    /* Transport status update */
    void transport_state(const TransportInfo &);

    void zrtpStatusSecureOn(int call_id, const QString &cipher);
    void zrtpStatusSecureOff(int call_id);
    void zrtpStatusShowSas(int call_id, const QString &sas, int verified);
};

struct AccountInfo {
    bool zrtpEnabled;
};

#endif /*PJCALLBACK_H_*/
