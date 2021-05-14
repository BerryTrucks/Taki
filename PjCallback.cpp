/* $Id: PjCallback.cpp */
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

#include "PjCallback.h"
#include "projectstrings.h"
//#include <libzrtpcpp/ZRtp.h>
#include <QList>
#include <QStringList>
#include <QMutex>
#include <QDebug>

#define THIS_FILE "PjCallback"

//static void ring_start(pjsua_call_id call_id);
//static void ring_stop(pjsua_call_id call_id);

//extern QList<int> activeCalls;
//extern QMutex activeCallsMutex;

/* global callback/logger object */
PjCallback *globalPjCallback;

#if defined(Q_OS_QNX)
const char *zrtpZidFile = "data/taki.zid";
#else
const char *zrtpZidFile = "taki.zid";
#endif

PjCallback::PjCallback()
{
    globalPjCallback = this;
}

PjCallback::~PjCallback()
{
    for (int i = 0; i < PJSUA_MAX_CALLS; ++i) {
        if (zrtpCbs[i]) {
            delete zrtpCbs[i];
        }
    }
}

void PjCallback::logger_cb(int level, const char *data, int len) {
    PJ_UNUSED_ARG(level);
    PJ_UNUSED_ARG(len);

    //qDebug() << "log:" << data;

    /* optional dump to stdout */
    //puts(data);
    /* emit signal with log message */
    /* paramter will be converted to QString which makes a deep copy */
    emit new_log_message(data);
}

void PjCallback::logger_cb_wrapper(int level, const char *data, int len)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->logger_cb(level, data, len);
    }
}

/* Notify application which NAT type was detected  */
void PjCallback::on_nat_detect(const pj_stun_nat_detect_result *res)
{
    QString description;
    switch (res->nat_type) {
    case PJ_STUN_NAT_TYPE_UNKNOWN:
        description="PJ_STUN_NAT_TYPE_UNKNOWN:\r\n\r\n"
                "NAT type is unknown because the detection has not been performed.";
        break;
    case PJ_STUN_NAT_TYPE_ERR_UNKNOWN:
        description="PJ_STUN_NAT_TYPE_ERR_UNKNOWN:\r\n\r\n"
                "NAT type is unknown because there is failure in the detection process, \r\n"
                "possibly because server does not support RFC 3489.";
        break;
    case PJ_STUN_NAT_TYPE_OPEN:
        description="PJ_STUN_NAT_TYPE_OPEN:\r\n\r\n"
                "This specifies that the client has open access to Internet (or at \r\n"
                "least, its behind a firewall that behaves like a full-cone NAT, but \r\n"
                "without the translation)";
        break;
    case PJ_STUN_NAT_TYPE_BLOCKED:
        description="PJ_STUN_NAT_TYPE_BLOCKED:\r\n\r\n"
                "This specifies that communication with server has failed, probably \r\n"
                "because UDP packets are blocked.";
        break;
    case PJ_STUN_NAT_TYPE_SYMMETRIC_UDP:
        description="PJ_STUN_NAT_TYPE_SYMMETRIC_UDP:\r\n\r\n"
                "Firewall that allows UDP out, and responses have to come back to the \r\n"
                "source of the request (like a symmetric NAT, but no translation.";
        break;
    case PJ_STUN_NAT_TYPE_FULL_CONE:
        description="PJ_STUN_NAT_TYPE_FULL_CONE:\r\n\r\n"
                "A full cone NAT is one where all requests from the same internal IP \r\n"
                "address and port are mapped to the same external IP address and port. \r\n"
                "Furthermore, any external host can send a packet to the internal host, \r\n"
                "by sending a packet to the mapped external address.";
        break;
    case PJ_STUN_NAT_TYPE_SYMMETRIC:
        description="PJ_STUN_NAT_TYPE_SYMMETRIC:\r\n\r\n"
                "A symmetric NAT is one where all requests from the same internal IP \r\n"
                "address and port, to a specific destination IP address and port, are \r\n"
                "mapped to the same external IP address and port. If the same host \r\n"
                "sends a packet with the same source address and port, but to a different \r\n"
                "destination, a different mapping is used. Furthermore, only the external \r\n"
                "host that receives a packet can send a UDP packet back to the internal host.";
        break;
    case PJ_STUN_NAT_TYPE_RESTRICTED:
        description="PJ_STUN_NAT_TYPE_RESTRICTED:\r\n\r\n"
                "A restricted cone NAT is one where all requests from the same internal \r\n"
                "IP address and port are mapped to the same external IP address and port. \r\n"
                "Unlike a full cone NAT, an external host (with IP address X) can send a \r\n"
                "packet to the internal host only if the internal host had previously \r\n"
                "sent a packet to IP address X.";
        break;
    case PJ_STUN_NAT_TYPE_PORT_RESTRICTED:
        description="PJ_STUN_NAT_TYPE_PORT_RESTRICTED:\r\n\r\n"
                "A port restricted cone NAT is like a restricted cone NAT, but the \r\n"
                "restriction includes port numbers. Specifically, an external host \r\n"
                "can send a packet, with source IP address X and source port P, to \r\n"
                "the internal host only if the internal host had previously sent a \r\n"
                "packet to IP address X and port P.: ";
        break;
    default:
        description="Error: unknown type detected!";
    }
    emit nat_detect(QString(res->nat_type_name), description);
}

void PjCallback::on_nat_detect_wrapper(const pj_stun_nat_detect_result *res)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_nat_detect(res);
    }
}

void PjCallback::on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    PJ_UNUSED_ARG(e);

    PJ_LOG(3,(THIS_FILE, "on_call_state"));
    /*
    activeCallsMutex.lock();
    if (activeCalls.empty()) {
        PJ_LOG(3,(THIS_FILE, "Call %d not found as callList is empty; new incoming call?", call_id));
    }
    if (!activeCalls.contains(call_id)) {
        PJ_LOG(3,(THIS_FILE, "Call %d not found in callList; new incoming call?", call_id));
    }
    activeCallsMutex.unlock();
    */

    pj_status_t status;
    pjsua_call_info ci;
    status = pjsua_call_get_info(call_id, &ci);
    if (status != PJ_SUCCESS) {
        PJ_LOG(3,(THIS_FILE, "ERROR retrieveing info for Call %d ... ignoring", call_id));
        return;
    }
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
              (int)ci.state_text.slen, ci.state_text.ptr));

    CallInfo callInfo(ci);

    if (ci.state == PJSIP_INV_STATE_EARLY) {
        int code;
        pjsip_msg *msg;

        /* This can only occur because of TX or RX message */
        pj_assert(e->type == PJSIP_EVENT_TSX_STATE);

        if (e->body.tsx_state.type == PJSIP_EVENT_RX_MSG) {
            msg = e->body.tsx_state.src.rdata->msg_info.msg;
        } else {
            msg = e->body.tsx_state.src.tdata->msg;
        }
        code = msg->line.status.code;

        /* Start ringback for 180 for UAC unless there's SDP in 180 */
        if (ci.role == PJSIP_ROLE_UAC && code == 180 &&
            msg->body == NULL &&
            ci.media_status == PJSUA_CALL_MEDIA_NONE)
        {
            callInfo.ring = true;
        }
    }

    emit setCallState(callInfo);

    /*
    activeCallsMutex.lock();
    switch(ci.state) {
    case PJSIP_INV_STATE_CALLING:
    case PJSIP_INV_STATE_INCOMING:
        activeCalls << call_id;
        break;
    case PJSIP_INV_STATE_DISCONNECTED:
        activeCalls.removeAt(activeCalls.indexOf(call_id));
        break;
    default:
        break;
    }
    activeCallsMutex.unlock();
    */
}

void PjCallback::on_call_state_wrapper(pjsua_call_id call_id, pjsip_event *e)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_call_state(call_id, e);
    }
}

void PjCallback::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pj_status_t status;
    pjsua_call_info ci;
    status = pjsua_call_get_info(call_id, &ci);
    if (status != PJ_SUCCESS) {
        PJ_LOG(3,(THIS_FILE, "ERROR retrieveing info for Call %d ... ignoring", call_id));
        return;
    }
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
              (int)ci.state_text.slen, ci.state_text.ptr));

    CallInfo callInfo(ci);
    emit setCallState(callInfo);
}

void PjCallback::on_incoming_call_wrapper(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_incoming_call(acc_id, call_id, rdata);
    }
}

void PjCallback::on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;
    pjsua_call_get_info(call_id, &ci);

    switch (ci.media_status) {
    case PJSUA_CALL_MEDIA_NONE:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                  "PJSUA_CALL_MEDIA_NONE: Call currently has no media", call_id));
        break;
    case PJSUA_CALL_MEDIA_REMOTE_HOLD:
    case PJSUA_CALL_MEDIA_ACTIVE:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                  "PJSUA_CALL_MEDIA_ACTIVE: The media is active", call_id));
        // When media is active, connect call to sound device.
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);
        break;
    case PJSUA_CALL_MEDIA_LOCAL_HOLD:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                  "PJSUA_CALL_MEDIA_LOCAL_HOLD: The media is currently put on hold by local endpoint", call_id));
        break;
    case PJSUA_CALL_MEDIA_ERROR:
        PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
                  "PJSUA_CALL_MEDIA_ERROR: The media has reported error (e.g. ICE negotiation)", call_id));
        break;
    }

    PJ_LOG(3,(THIS_FILE, "Call %d media media_status=%d", call_id, ci.media_status));
    emit setMediaState(call_id, ci.media_status);
}

void PjCallback::on_call_media_state_wrapper(pjsua_call_id call_id)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_call_media_state(call_id);
    }
}

void PjCallback::on_buddy_state(pjsua_buddy_id buddy_id)
{
    PJ_LOG(3,(THIS_FILE, "on_buddy_state called for buddy_id %d", buddy_id));
    emit buddy_state(buddy_id);
}

void PjCallback::on_buddy_state_wrapper(pjsua_buddy_id buddy_id)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_buddy_state(buddy_id);
    }
}

void PjCallback::on_reg_state2(pjsua_acc_id acc_id, pjsua_reg_info *info)
{
    RegInfo ri(acc_id, info);
    //qDebug() << "PjCallback::on_reg_state2" << endl << ri;
    emit reg_state(ri);
}

void PjCallback::on_reg_state_wrapper2(pjsua_acc_id acc_id, pjsua_reg_info *info)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_reg_state2(acc_id, info);
    }
}

void PjCallback::on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info)
{
    int numNewMsg = 0;
    int numOldMsg = 0;
    bool mw = false;
    //QString mwiAccount;

    if (!mwi_info || !mwi_info->rdata || !mwi_info->rdata->msg_info.msg->body)
    {
        return;
    }

    QString msgBody = QString::fromAscii((char *)mwi_info->rdata->msg_info.msg->body->data,
                                 mwi_info->rdata->msg_info.msg->body->len);

    QTextStream msgStream(&msgBody);
    QString line;
    do {
        line = msgStream.readLine();
        if (line.isEmpty()) {
            continue;
        }
        QStringList list = line.split(" ");
        if (list.size() < 2) {
            continue;
        }
        if (QString::compare(list[0], "Messages-Waiting:", Qt::CaseInsensitive) == 0  &&
            QString::compare(list[1], "yes", Qt::CaseInsensitive) == 0)
        {
            mw = true;
            continue;
        }
        if (QString::compare(list[0], "Voice-Message:", Qt::CaseInsensitive) == 0) {
            QString mwiNumMsg = list[1];
            if (!mwiNumMsg.isEmpty()) {
                QStringList numMsgList = mwiNumMsg.split("/");
                if (numMsgList.size() == 2) {
                    numNewMsg = numMsgList[0].toInt();
                    numOldMsg = numMsgList[1].toInt();
                }
            }
            continue;
        }
    } while (!line.isNull());
    //qDebug() << "on_mwi_info:" << mw << numNewMsg << numOldMsg;// << mwiAccount;
    emit mwi_state(acc_id, mw, numNewMsg, numOldMsg);
}

void PjCallback::on_mwi_info_wrapper(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_mwi_info(acc_id, mwi_info);
    }
}

/* Notification on incoming request
 * Handle requests which are unhandled by pjsua, eg. incoming
 * PUBLISH, NOTIFY w/o SUBSCRIBE, ...
 */
pj_bool_t PjCallback::default_mod_on_rx_request(pjsip_rx_data *rdata)
{
    pjsip_tx_data *tdata;
    pjsip_status_code status_code;
    pj_status_t status;

    //qDebug() << "default_mod_on_rx_request";

    /* Don't respond to ACK! */
    if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,
                         &pjsip_ack_method) == 0)
        return PJ_TRUE;

    /* Create basic response. */
    if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,
                         &pjsip_notify_method) == 0) {
        /* Unsolicited NOTIFY's, send with Bad Request */
        status_code = PJSIP_SC_BAD_REQUEST;
    } else {
        /* Probably unknown method */
        status_code = PJSIP_SC_METHOD_NOT_ALLOWED;
    }
    status = pjsip_endpt_create_response(pjsua_get_pjsip_endpt(), rdata,
                                         status_code, NULL, &tdata);
    if (status != PJ_SUCCESS) {
        //logErrMsg("Unable to create response", status);
        PJ_LOG(3,(THIS_FILE, "Unable to create response %d", status));
        return PJ_TRUE;
    }

    /* Add Allow if we're responding with 405 */
    if (status_code == PJSIP_SC_METHOD_NOT_ALLOWED) {
        const pjsip_hdr *cap_hdr;
        cap_hdr = pjsip_endpt_get_capability(pjsua_get_pjsip_endpt(),
                                             PJSIP_H_ALLOW, NULL);
        if (cap_hdr) {
            pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)pjsip_hdr_clone(tdata->pool, cap_hdr));
        }
    }

    /* Add User-Agent header */
    {
        pj_str_t usrAg;
        pj_cstr(&usrAg, "User-Agent");

        pj_str_t usrAgName;
        pj_cstr(&usrAgName, ProjectStrings::userAgent);

        pjsip_hdr *h = (pjsip_hdr*) pjsip_generic_string_hdr_create(tdata->pool,
                                                                    &usrAg,
                                                                    &usrAgName);
        pjsip_msg_add_hdr(tdata->msg, h);
    }

    pjsip_endpt_send_response2(pjsua_get_pjsip_endpt(), rdata, tdata, NULL,
                               NULL);

    return PJ_TRUE;
}

pj_bool_t PjCallback::default_mod_on_rx_request_wrapper(pjsip_rx_data *rdata)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->default_mod_on_rx_request(rdata);
    }
    return PJ_TRUE;
}

void PjCallback::on_call_transfer_status(pjsua_call_id call_id, int st_code,
                                         const pj_str_t *st_text, pj_bool_t final,
                                         pj_bool_t *p_cont)
{
    PJ_UNUSED_ARG(p_cont);

    QString statusText(QString::fromAscii(st_text->ptr, (int)st_text->slen));

    /*
    qDebug() << "on_call_transfer_status" <<
                call_id << st_code << statusText << final;
    PJ_LOG(3,(THIS_FILE, "on_call_transfer_status called for call %d", call_id));
    */

    emit call_transfer_status(call_id, st_code, statusText, final);
}

void PjCallback::on_call_transfer_status_wrapper(pjsua_call_id call_id, int st_code,
                                                 const pj_str_t *st_text, pj_bool_t final,
                                                 pj_bool_t *p_cont)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_call_transfer_status(call_id, st_code, st_text, final, p_cont);
    }
}

void PjCallback::on_call_tsx_state(pjsua_call_id call_id, pjsip_transaction *tsx, pjsip_event *e)
{
    PJ_UNUSED_ARG(e);
    /*
    qDebug() <<  "on_call_tsx_state" << call_id;
    qDebug() <<  tsx->status_code
              << QString::fromAscii(tsx->status_text.ptr, tsx->status_text.slen);
    qDebug() <<  QString::fromAscii(tsx->method.name.ptr, tsx->method.name.slen);
    */

    // Handle transfer errors unhandled in on_call_transfer_status
    if (tsx->status_code >= 500 &&
            pjsip_method_cmp(&tsx->method, pjsip_get_refer_method()) == 0)
    {
        QString statusText(QString::fromAscii(tsx->status_text.ptr, tsx->status_text.slen));
        emit call_transfer_status(call_id, tsx->status_code, statusText, true);
    }
}

void PjCallback::on_call_tsx_state_wrapper(pjsua_call_id call_id, pjsip_transaction *tsx, pjsip_event *e)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_call_tsx_state(call_id, tsx, e);
    }
}

#if 0
void PjCallback::on_pager(pjsua_call_id call_id, const pj_str_t *from,
                          const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
                          const pj_str_t *text)
{
    /* Note: call index may be -1 */
    PJ_UNUSED_ARG(call_id);
    PJ_UNUSED_ARG(to);
    PJ_UNUSED_ARG(contact);
    PJ_UNUSED_ARG(mime_type);

    emit new_im(QString::fromAscii(from->ptr,from->slen),
                QString::fromAscii(text->ptr,text->slen));
}

/* callback wrapper function called by pjsip
 * Incoming IM message (i.e. MESSAGE request)! */
void PjCallback::on_pager_wrapper(pjsua_call_id call_id, const pj_str_t *from,
                                  const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
                                  const pj_str_t *text)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_pager(call_id, from, to, contact, mime_type, text);
    }
}
#endif

//void PjCallback::on_pager2(pjsua_call_id call_id, const pj_str_t *from,
//		const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
//		const pj_str_t *text, pjsip_rx_data *rdata) {
//	/* Note: call index may be -1 */
//	PJ_UNUSED_ARG(call_id);
//	PJ_UNUSED_ARG(to);
//	PJ_UNUSED_ARG(contact);
//	PJ_UNUSED_ARG(mime_type);
//	PJ_UNUSED_ARG(rdata);
//	
//	char *cfrom, *ctext, *cfromuri;
//	cfrom = (char*) malloc((from->slen)+1);
//	if (!cfrom) {
//		puts("cfrom memory allocation error");
//		return;
//	}
//	memcpy(cfrom, from->ptr, from->slen);
//	memcpy(cfrom+(from->slen), "", 1);
//	ctext = (char*) malloc((text->slen)+1);
//	if (!ctext) {
//		puts("ctext memory allocation error");
//		return;
//	}
//	memcpy(ctext, text->ptr, text->slen);
//	memcpy(ctext+(text->slen), "", 1);
//
////rdata->msg_info.from->uri->vptr->
////	cfromuri = (char*) malloc((from->slen)+1);
////	if (!cfromuri) {
////		puts("cfrom memory allocation error");
////		return;
////	}
////	memcpy(cfromuri, from->ptr, from->slen);
////	memcpy(cfromuri+(from->slen), "", 1);
//
////	emit new_im(QString(cfrom), QString(cfromuri), QString(ctext));
//	emit new_im(QString(cfrom), QString(ctext));
//}
//

void PjCallback::on_pager2(pjsua_call_id call_id, const pj_str_t *from,
                           const pj_str_t *to, const pj_str_t *contact,
                           const pj_str_t *mime_type, const pj_str_t *text,
                           pjsip_rx_data *rdata, pjsua_acc_id acc_id)
{
    //qDebug() << "on_pager2 ...";
    Q_UNUSED(call_id);
    Q_UNUSED(contact);
    Q_UNUSED(mime_type);
    Q_UNUSED(rdata);

    // Ignore empty message
    if (to->slen == 0 || text->slen == 0)
        return;

    MessageRec *msg = new MessageRec;
    msg->from = QString::fromAscii(from->ptr, from->slen);
    msg->to.setPhoneNumber(QString::fromAscii(to->ptr, to->slen));
    msg->direction = MessageRec::In;
    msg->deliveryStatus = MessageRec::Sent;
    msg->setChatId();
    msg->contact = QString::fromAscii(contact->ptr, contact->slen);
    msg->text = QString::fromAscii(text->ptr, text->slen);
    msg->pjSipAccId = acc_id;

    //qDebug() << "PjCallback::on_pager2 received message:" << *msg;

    emit on_pager(msg);
}

/* Incoming IM message (i.e. MESSAGE request)! */
void PjCallback::on_pager2_wrapper(pjsua_call_id call_id, const pj_str_t *from,
                                   const pj_str_t *to, const pj_str_t *contact,
                                   const pj_str_t *mime_type, const pj_str_t *text,
                                   pjsip_rx_data *rdata, pjsua_acc_id acc_id)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_pager2(call_id, from, to, contact,
                        mime_type, text, rdata, acc_id);
    }
}

void PjCallback::on_pager_status2(pjsua_call_id call_id, const pj_str_t *to,
                                  const pj_str_t *body, void *user_data,
                                  pjsip_status_code status, const pj_str_t *reason,
                                  pjsip_tx_data *tdata, pjsip_rx_data *rdata,
                                  pjsua_acc_id acc_id)
{
    //qDebug() << "on_pager_status2 ...";

    Q_UNUSED(call_id);
    Q_UNUSED(to);
    Q_UNUSED(body);
    Q_UNUSED(tdata);
    Q_UNUSED(rdata);

    MessageRec msg;
    quint32 *pId = static_cast<quint32*>(user_data);
    msg.id = *pId;
    msg.pjSipAccId = acc_id;
    msg.pjStatusCode = status;
    msg.pjStatusText = QString::fromAscii(reason->ptr, reason->slen);
    if (status >= 400) {
        msg.deliveryStatus = MessageRec::Failed;
    }
    else {
        msg.deliveryStatus = MessageRec::Sent;
    }

    //qDebug() << "PjCallback::on_pager_status2 messge address:" << &msg;

    emit on_pager_status(msg);
}

void PjCallback::on_pager_status2_wrapper(pjsua_call_id call_id, const pj_str_t *to,
                                          const pj_str_t *body, void *user_data,
                                          pjsip_status_code status, const pj_str_t *reason,
                                          pjsip_tx_data *tdata, pjsip_rx_data *rdata,
                                          pjsua_acc_id acc_id)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_pager_status2(call_id, to, body, user_data,
                               status, reason, tdata, rdata, acc_id);
    }
}

void PjCallback::on_typing2(pjsua_call_id call_id, const pj_str_t *from,
                            const pj_str_t *to, const pj_str_t *contact,
                            pj_bool_t is_typing, pjsip_rx_data *rdata,
                            pjsua_acc_id acc_id)
{
    Q_UNUSED(call_id);
    Q_UNUSED(from);
    Q_UNUSED(to);
    Q_UNUSED(contact);
    Q_UNUSED(is_typing);
    Q_UNUSED(rdata);
    Q_UNUSED(acc_id);
}

void PjCallback::on_typing2_wrapper(pjsua_call_id call_id, const pj_str_t *from,
                                    const pj_str_t *to, const pj_str_t *contact,
                                    pj_bool_t is_typing, pjsip_rx_data *rdata,
                                    pjsua_acc_id acc_id)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_typing2(call_id, from, to, contact,
                         is_typing, rdata, acc_id);
    }
}

void PjCallback::on_transport_state(pjsip_transport *tp,
                                    pjsip_transport_state state,
                                    const pjsip_transport_state_info *info)
{
    TransportInfo ti(tp, state, info);
    //qDebug() << Q_FUNC_INFO << endl << ti;
    emit transport_state(ti);
}

void PjCallback::on_transport_state_wrapper(pjsip_transport *tp,
                                            pjsip_transport_state state,
                                            const pjsip_transport_state_info *info)
{
    /* call the non-static member */
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->on_transport_state(tp, state, info);
    }
}

/**
 * ZRTP constants
 */
const char* InfoCodes[] =
{
    "EMPTY",
    "Hello received, preparing a Commit",
    "Commit: Generated a public DH key",
    "Responder: Commit received, preparing DHPart1",
    "DH1Part: Generated a public DH key",
    "Initiator: DHPart1 received, preparing DHPart2",
    "Responder: DHPart2 received, preparing Confirm1",
    "Initiator: Confirm1 received, preparing Confirm2",
    "Responder: Confirm2 received, preparing Conf2Ack",
    "At least one retained secrets matches - security OK",
    "Entered secure state",
    "No more security for this session"
};

/**
 * Sub-codes for Warning
 */
const char* WarningCodes [] =
{
    "EMPTY",
    "Commit contains an AES256 cipher but does not offer a Diffie-Helman 4096",
    "Received a GoClear message",
    "Hello offers an AES256 cipher but does not offer a Diffie-Helman 4096",
    "No retained shared secrets available - must verify SAS",
    "Internal ZRTP packet checksum mismatch - packet dropped",
    "Dropping packet because SRTP authentication failed!",
    "Dropping packet because SRTP replay check failed!",
    "Valid retained shared secrets availabe but no matches found - must verify SAS"
};

/**
 * Sub-codes for Severe
 */
const char* SevereCodes[] =
{
    "EMPTY",
    "Hash HMAC check of Hello failed!",
    "Hash HMAC check of Commit failed!",
    "Hash HMAC check of DHPart1 failed!",
    "Hash HMAC check of DHPart2 failed!",
    "Cannot send data - connection or peer down?",
    "Internal protocol error occured!",
    "Cannot start a timer - internal resources exhausted?",
    "Too much retries during ZRTP negotiation - connection or peer down?"
};

pjsua_call_id PjCallback::getCallId(void *data) const
{
    pjsua_call_id call_id = PJSUA_INVALID_ID;
    for (int i = PJSUA_MAX_CALLS-1; i >= 0; --i) {
        if (zrtpCbs[i] && zrtpCbs[i]->userData == data) {
            call_id = i;
            break;
        }
    }
    //qDebug () << "Found call:" << call_id;
    return call_id;
}

void PjCallback::secureOn_wrapper(void* data, char* cipher)
{
    PJ_LOG(3,(THIS_FILE, "Secure On, cipher: %s", cipher));
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->secureOn(data, cipher);
    }
}

void PjCallback::secureOn(void* data, char* cipher)
{
    pjmedia_transport *tp = (pjmedia_transport *)data;
    qDebug() << "Secure On, cipher:" << cipher <<  "Tr name:" << tp->name;

    pjsua_call_id call_id = getCallId(data);
    if (call_id == PJSUA_INVALID_ID) {
        qDebug() << "Can't find a call";
        return;
    }
    QString c(cipher);
    emit zrtpStatusSecureOn(call_id, c);
}

void PjCallback::secureOff_wrapper(void* data)
{
    PJ_LOG(3,(THIS_FILE, "Secure Off"));
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->secureOff(data);
    }
}

void PjCallback::secureOff(void* data)
{
    qDebug() << Q_FUNC_INFO << "Security disabled";
    pjsua_call_id call_id = getCallId(data);
    if (call_id == PJSUA_INVALID_ID) {
        qDebug() << "Can't find a call";
        return;
    }
    emit zrtpStatusSecureOff(call_id);
}

void PjCallback::showSAS_wrapper(void* data, char* sas, int32_t verified)
{
    PJ_LOG(3,(THIS_FILE, "SAS data: %s, verified: %d", sas, verified));
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->showSAS(data, sas, verified);
    }
}

void PjCallback::showSAS(void* data, char* sas, int32_t verified)
{
    qDebug() << "SAS data:" << sas << "verified:" << verified;
    pjsua_call_id call_id = getCallId(data);
    if (call_id == PJSUA_INVALID_ID) {
        qDebug() << "Can't find call";
        return;
    }

    QString s(sas);
    emit zrtpStatusShowSas(call_id, s, verified);
}

void PjCallback::confirmGoClear(void* data)
{
    PJ_LOG(3,(THIS_FILE, "ZRTP - Go Clear?"));
    Q_UNUSED(data);
}

void PjCallback::showMessage_wrapper(void* data, int32_t sev, int32_t subCode)
{
    if (globalPjCallback) {
        PjCallback *myCb = (PjCallback*) globalPjCallback;
        myCb->showMessage(data, sev, subCode);
    }
}

void PjCallback::showMessage(void* data, int32_t sev, int32_t subCode)
{
    Q_UNUSED(data);
    switch (sev)
    {
        case zrtp_Info:
            PJ_LOG(3,(THIS_FILE, "ZRTP info message: %s", InfoCodes[subCode]));
            break;

        case zrtp_Warning:
            PJ_LOG(3,(THIS_FILE, "ZRTP warning message: %s", WarningCodes[subCode]));
            break;

        case zrtp_Severe:
            PJ_LOG(3,(THIS_FILE, "ZRTP severe message: %s", SevereCodes[subCode]));
            break;

        case zrtp_ZrtpError:
            PJ_LOG(3,(THIS_FILE, "ZRTP Error: severity: %d, subcode: %x", sev, subCode));
            break;
    }
}

void PjCallback::zrtpNegotiationFailed(void* data, int32_t severity, int32_t subCode)
{
    PJ_LOG(3,(THIS_FILE, "ZRTP failed: %d, subcode: %d", severity, subCode));
    Q_UNUSED(data);
}

void PjCallback::zrtpNotSuppOther(void* data)
{
    PJ_LOG(3,(THIS_FILE, "ZRTP not supported by other peer"));
    Q_UNUSED(data);
}

void PjCallback::zrtpAskEnrollment(void* data, int32_t info)
{
    PJ_LOG(3,(THIS_FILE, "ZRTP - Ask PBX enrollment"));
    Q_UNUSED(data);
    Q_UNUSED(info);
}

void PjCallback::zrtpInformEnrollment(void* data, int32_t info)
{
    PJ_LOG(3,(THIS_FILE, "ZRTP - Inform PBX enrollement"));
    Q_UNUSED(data);
    Q_UNUSED(info);
}

void PjCallback::signSAS(void* data, unsigned char* sas)
{
    PJ_LOG(3,(THIS_FILE, "ZRTP - sign SAS"));
    Q_UNUSED(data);
    Q_UNUSED(sas);
}

int32_t PjCallback::checkSASSignature(void* data, unsigned char* sas)
{
    PJ_LOG(3,(THIS_FILE, "ZRTP - check SAS signature"));
    Q_UNUSED(data);
    Q_UNUSED(sas);
    return 0;
}

zrtp_UserCallbacks * PjCallback::zrtpCbs[PJSUA_MAX_CALLS] = {};

#if 0
pjmedia_transport* PjCallback::on_create_media_transport(
        pjsua_call_id call_id,
        unsigned media_idx,
        pjmedia_transport *base_tp,
        unsigned flags)
{
    return base_tp;
}
#else
pjmedia_transport* PjCallback::on_create_media_transport(
        pjsua_call_id call_id,
        unsigned media_idx,
        pjmedia_transport *base_tp,
        unsigned flags)
{
    PJ_LOG(3,(THIS_FILE, "on_create_media_transport"));
    qDebug() << Q_FUNC_INFO << call_id << "media_idx" << media_idx << "flags" << flags;
    Q_UNUSED(media_idx);

    pj_status_t status;
    pjsua_call_info ci;
    status = pjsua_call_get_info(call_id, &ci);
    if (status != PJ_SUCCESS) {
        PJ_LOG(3,(THIS_FILE, "Error getting call info. Using base transport"));
        return base_tp;
    }

    AccountInfo *ai = static_cast<AccountInfo *>(pjsua_acc_get_user_data(ci.acc_id));
    if (ai && ai->zrtpEnabled) {
        pjmedia_transport *zrtp_tp = NULL;
        pjmedia_endpt* endpt = pjsua_get_pjmedia_endpt();
        if (!endpt) {
            PJ_LOG(3,(THIS_FILE, "Can't get media endpoint."));
            return NULL;
        }

        status = pjmedia_transport_zrtp_create(endpt, NULL, base_tp, &zrtp_tp, flags);
        if (status != PJ_SUCCESS) {
            PJ_LOG(3,(THIS_FILE, "Error creating zrtp transport."));
            return NULL;
        }
        else {
            PJ_LOG(3,(THIS_FILE, "ZRTP transport created"));
        }

        // Setup ZRTP callbacks per call
        if (zrtpCbs[call_id]) {
            delete zrtpCbs[call_id];
        }
        zrtpCbs[call_id] = new zrtp_UserCallbacks;
        if (!zrtpCbs[call_id]) {
            PJ_LOG(3,(THIS_FILE, "Error creating zrtp callback structure."));
            return NULL;
        }
        zrtpCbs[call_id]->zrtp_secureOn = &PjCallback::secureOn_wrapper;
        zrtpCbs[call_id]->zrtp_showSAS = &PjCallback::showSAS_wrapper;
        zrtpCbs[call_id]->zrtp_secureOff = &PjCallback::secureOff_wrapper;
        zrtpCbs[call_id]->zrtp_confirmGoClear = &PjCallback::confirmGoClear;
        zrtpCbs[call_id]->zrtp_showMessage = &PjCallback::showMessage_wrapper;
        zrtpCbs[call_id]->zrtp_zrtpNegotiationFailed = &PjCallback::zrtpNegotiationFailed;
        zrtpCbs[call_id]->zrtp_zrtpNotSuppOther = &PjCallback::zrtpNotSuppOther;
        zrtpCbs[call_id]->zrtp_zrtpAskEnrollment = &PjCallback::zrtpAskEnrollment;
        zrtpCbs[call_id]->zrtp_zrtpInformEnrollment = &PjCallback::zrtpInformEnrollment;
        zrtpCbs[call_id]->zrtp_signSAS = &PjCallback::signSAS;
        zrtpCbs[call_id]->zrtp_checkSASSignature = &PjCallback::checkSASSignature;
        zrtpCbs[call_id]->userData = zrtp_tp;

        pjmedia_transport_zrtp_setUserCallback(zrtp_tp, zrtpCbs[call_id]);
        pjsua_call_set_user_data(call_id, zrtp_tp);

        /*
         * Initialize the transport. Just the filename of the ZID file that holds
         * our partners ZID, shared data etc. If the files does not exists it will
         * be created an initialized.
         */
        pjmedia_transport_zrtp_initialize(zrtp_tp, zrtpZidFile, PJ_TRUE);

        return zrtp_tp;
    }
    return base_tp;
}
#endif
