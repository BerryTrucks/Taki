/* $Id: messagerec.cpp */
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

#include "messagerec.h"
#include <QRegExp>

const quint32 MessageRec::streamHeader = 0xFAEAFACEU;
const quint16 MessageRec::classVersion = 1;

MessageRec::MessageRec() :
    id(0),
    chatContact(),
    from(""),
    to(),
    contact(""),
    text(""),
    timeStamp(QDateTime::currentDateTime()),
    direction(Out),
    readStatus(UnRead),
    deliveryStatus(New),
    pjSipAccId(-1),
    pjStatusCode(0),
    pjStatusText(""),
#if defined(Q_OS_QNX) && defined(BB10_BLD)
    nt(NULL),
#endif
    m_errorString("")
{
}

MessageRec::MessageRec(const MessageRec& other) :
   id(other.id),
   chatContact(other.chatContact),
   from(other.from),
   to(other.to),
   contact(other.contact),
   text(other.text),
   timeStamp(other.timeStamp),
   direction(other.direction),
   readStatus(other.readStatus),
   deliveryStatus(other.deliveryStatus),
   pjSipAccId(other.pjSipAccId),
   pjStatusCode(other.pjStatusCode),
   pjStatusText(other.pjStatusText),
#if defined(Q_OS_QNX) && defined(BB10_BLD)
   nt(NULL),
#endif
   m_errorString(other.m_errorString)
{
    //qDebug() << "MessageRec copy constructor called";
}

MessageRec::~MessageRec() {
    notifyStop();
}

void MessageRec::setChatId()
{
    QString chatId;

    // Extract chat id
    if (from.isEmpty()) {
        chatId = to.getPhoneNumber();
    }
    else {
        QRegExp rxAddr("<.+:(.+)@.+>");
        if (rxAddr.indexIn(from) != -1) {
            chatId = rxAddr.cap(1);
        }
        else {
            QRegExp rxAddr1(".+:(.+)@.+");
            if (rxAddr1.indexIn(from) != -1) {
                chatId = rxAddr1.cap(1);
            }
        }
    }
    chatContact.setPhoneNumber(chatId);
}

void MessageRec::insertToDataStream(QDataStream& dataStream) const
{
    dataStream << streamHeader << classVersion;

    // Stream enumerated types as qint32
    const qint32 direction_int = static_cast<qint32>(direction);
    const qint32 read_status_int = static_cast<qint32>(readStatus);
    const qint32 delivery_status_int = static_cast<qint32>(deliveryStatus);

    // Added in classVersion = 1
    dataStream << id
               << chatContact
               << from
               << to.getPhoneNumber()
               << contact
               << text
               << timeStamp
               << direction_int
               << read_status_int
               << delivery_status_int
               << pjSipAccId
               << pjStatusCode
               << pjStatusText;

    // Added in classVersion = 2
    //dataStream << avatar;
}

void MessageRec::extractFromDataStream(QDataStream& dataStream)
{
    quint32 actualStreamHeader = 0;
    dataStream >> actualStreamHeader;

    if (actualStreamHeader != streamHeader) {
        m_errorString = QString(
            "MessageRec::extractFromDataStream() failed.\n"
            "MessageRec prefix mismatch error: actualStreamHeader = 0x%1 and streamHeader = 0x%2" )
                .arg( actualStreamHeader, 8, 0x10, QChar( '0' ) )
                .arg( streamHeader, 8, 0x10, QChar( '0' ) );
        return;
    }

    quint16 actualClassVersion = 0;
    dataStream >> actualClassVersion;

    if (actualClassVersion > classVersion) {
        m_errorString = QString(
            "UserRecord::extractFromDataStream() failed.\n"
            "UserRecord compatibility error: actualClassVersion = %1 and classVersion = %2" )
                .arg( actualClassVersion ).arg( classVersion );
        return;
    }

    qint32 direction_int = 0;
    qint32 read_status_int = 0;
    qint32 delivery_status_int = 0;
    QString toPhoneNumber;
    dataStream >> id
               >> chatContact
               >> from
               >> toPhoneNumber
               >> contact
               >> text
               >> timeStamp
               >> direction_int
               >> read_status_int
               >> delivery_status_int
               >> pjSipAccId
               >> pjStatusCode
               >> pjStatusText;
    to.setPhoneNumber(toPhoneNumber);
    direction = static_cast<Direction>(direction_int);
    readStatus = static_cast<ReadStatus>(read_status_int);
    deliveryStatus = static_cast<DeliveryStatus>(delivery_status_int);

    //if ( actualClassVersion >= 2 )
    //    dataStream >> avatar;
}

void MessageRec::notifyStart()
{
    //qDebug() << "MessageRec::notifyStart";

#if defined(Q_OS_QNX) && defined(BB10_BLD)
    if (nt) {
        return;
    }
    if (direction == Out) {
        return;
    }

    int ret = notification_message_create(&nt);
    if (ret != BPS_SUCCESS) {
         qWarning() << "Error creating notification event";
         return;
    }
    //qDebug() << "start notification with id" << id;

    char itemId[20];
    snprintf(itemId, sizeof(itemId), "%d", id);
    notification_message_set_item_id(nt, itemId);

    //notification_message_set_title(nt, "Taki. New instant message");
    //notification_message_set_subtitle(nt, "this is a messag textsubtitle text");
    //notification_message_set_sound_url(nt,"notification_text_message_im_received");
    //"/base/usr/share/sounds/notification_text_message_im_received.wav");

    notification_notify(nt);
#endif
}

void MessageRec::notifyStop()
{
    //qDebug() << "MessageRec::notifyStop";

#if defined(Q_OS_QNX) && defined(BB10_BLD)
    if (nt) {
        //qDebug() << "MessageRec::notifyStop stopped for id:" << id;
        int ret = notification_cancel(nt);
        if (ret != BPS_SUCCESS) {
             qWarning() << "Error canceling notification event";
             return;
        }
        notification_message_destroy(&nt);
        nt = NULL;
    }
#endif
}

void MessageRec::setReadStatus(ReadStatus rs)
{
    if (rs == Read) {
        notifyStop();
    }
    if (rs == UnRead && direction == In) {
        notifyStart();
    }
    readStatus = rs;
}

QDataStream& operator<<(QDataStream& dataStream, const MessageRec& msg)
{
    msg.insertToDataStream(dataStream);
    return dataStream;
}

QDataStream& operator>>(QDataStream& dataStream, MessageRec& msg)
{
    msg.extractFromDataStream(dataStream);
    return dataStream;
}

QDataStream& operator<<(QDataStream& dataStream, const MessageRec *msg)
{
    msg->insertToDataStream(dataStream);
    return dataStream;
}

QDataStream& operator>>(QDataStream& dataStream, MessageRec *&msg)
{
    msg = new MessageRec;
    msg->extractFromDataStream(dataStream);
    return dataStream;
}

QDebug operator<<(QDebug dbg, const MessageRec &msg)
{
    dbg.maybeSpace() << "Id:" << msg.id << "\n";
    dbg.maybeSpace() << "chatContact:" << msg.chatContact << "\n";
    dbg.maybeSpace() << "From:" << msg.from << "\n";
    dbg.maybeSpace() << "To:" << msg.to.getPhoneNumber() << "\n";
    dbg.maybeSpace() << "Contact:" << msg.contact << "\n";
    dbg.maybeSpace() << "Text:" << msg.text << "\n";
    dbg.maybeSpace() << "Time Stamp:" << msg.timeStamp.toString() << "\n";
    dbg.maybeSpace() << "ToDirectCallUri:" << msg.to.getDirectCallUri() << "\n";

    dbg.maybeSpace() << "Direction:";
    switch (msg.direction) {
    case MessageRec::In:
        dbg.maybeSpace() << "In";
        break;
    case MessageRec::Out:
        dbg.maybeSpace() << "Out";
        break;
    }
    dbg.maybeSpace() << "\n";

    dbg.maybeSpace() << "ReadStatus:";
    switch (msg.readStatus) {
    case MessageRec::Read:
        dbg.maybeSpace() << "Read";
        break;
    case MessageRec::UnRead:
        dbg.maybeSpace() << "UnRead";
        break;
    }
    dbg.maybeSpace() << "\n";

    dbg.maybeSpace() << "DeliveryStatus:";
    switch (msg.deliveryStatus) {
    case MessageRec::New:
        dbg.maybeSpace() << "New";
        break;
    case MessageRec::Queued:
        dbg.maybeSpace() << "Queued";
        break;
    case MessageRec::Sent:
        dbg.maybeSpace() << "Sent";
        break;
    case MessageRec::Failed:
        dbg.maybeSpace() << "Failed";
        break;
    case MessageRec::Delivered:
        dbg.maybeSpace() << "Delivered";
        break;
    }
    dbg.maybeSpace() << "\n";

    dbg.maybeSpace() << "pjStatusCode:" << msg.pjStatusCode << "\n";
    dbg.maybeSpace() << "pjStatusText:" << msg.pjStatusText << "\n";

    return dbg.maybeSpace();
}
