/* $Id: messagerec.h */
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

#ifndef MESSAGEREC_H
#define MESSAGEREC_H

#include "cont.h"
#include <QDateTime>
#include <QDataStream>
#include <QMetaType>
#include <QDebug>
#if defined(Q_OS_QNX)
#ifdef BB10_BLD
#include <time.h>
#include <bps/notification.h>
#endif
#endif

class MessageRec
{
public:
    enum Direction {
        In,
        Out
    };
    enum ReadStatus {
        UnRead,
        Read
    };
    enum DeliveryStatus {
        New,
        Queued,
        Sent,
        Failed,
        Delivered
    };

public:
    MessageRec();
    MessageRec(const MessageRec& other);
    ~MessageRec();
    void insertToDataStream(QDataStream& dataStream) const;
    void extractFromDataStream(QDataStream& dataStream);
    const QString & errorString(void) const { return m_errorString; }
    friend QDebug operator<<(QDebug dbg, const MessageRec &msg);

    quint32 id;
    Cont chatContact;
    QString from;
    Cont to;
    QString contact;
    QString text;
    QDateTime timeStamp;
    Direction direction;
    ReadStatus readStatus;
    DeliveryStatus deliveryStatus;
    int pjSipAccId;
    int pjStatusCode;
    QString pjStatusText;
#if defined(Q_OS_QNX) && defined(BB10_BLD)
    notification_message_t *nt;
#endif

    void setChatId();
    void setReadStatus(ReadStatus rs);
    void notifyStart();
    void notifyStop();

private:
    QString m_errorString;
    static const quint32 streamHeader;
    static const quint16 classVersion;
};

QDataStream& operator<<(QDataStream& dataStream, const MessageRec& msg);
QDataStream& operator>>(QDataStream& dataStream, MessageRec& msg);
QDataStream& operator<<(QDataStream& dataStream, const MessageRec *msg);
QDataStream& operator>>(QDataStream& dataStream, MessageRec *&msg);

#endif // MESSAGEREC_H
