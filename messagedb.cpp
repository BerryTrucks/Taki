/* $Id: messagedb.cpp */
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

#include "messagedb.h"
#include "messagefile.h"
#include <QDebug>

#if defined(Q_OS_QNX)
const char *msgDBFile = "data/messages.dat";
#else
const char *msgDBFile = "messages.dat";
#endif

MessageDB::MessageDB(QObject *parent) :
    QObject(parent),
    lastMsgId(0)
{
    load();
}

MessageDB::~MessageDB()
{
    qDeleteAll(messageList);
    messageList.clear();
}

void MessageDB::onMsgStatus(const MessageRec &msgStatus)
{
    //qDebug() << "MessageDB::onMsgStatus" << msgStatus;

    MessageRec *msg = messageList.findById(msgStatus.id);
    if (msg) {
        msg->pjStatusCode = msgStatus.pjStatusCode;
        msg->pjStatusText = msgStatus.pjStatusText;
        msg->deliveryStatus = msgStatus.deliveryStatus;
    }
    save();
}

void MessageDB::onMsgNew(MessageRec *msg)
{
    //qDebug() << "MessageDB::onMsgNew Adding msg to the list";

    if (msg->direction == MessageRec::In) {
        msg->deliveryStatus = MessageRec::Delivered;
    }
    messageList.append(msg);
    save();
}

void MessageDB::onMsgDeleteChat(const Cont &chatContact)
{
    bool removed = false;
    QString chatPhone(chatContact.getPhoneNumber());

    MessageRecList::iterator it = messageList.begin();
    while (it != messageList.end()) {
        MessageRec *msg = *it;
        if (msg->chatContact.getPhoneNumber() == chatPhone) {
            it = messageList.erase(it);
            delete msg;
            removed = true;
        }
        else {
            ++it;
        }
    }
    if (removed) {
        save();
    }
}

void MessageDB::onMsgDeleteAll()
{
    qDeleteAll(messageList);
    messageList.clear();
    save();
}

void MessageDB::onMsgUpdated()
{
    save();
}

void MessageDB::load()
{
    MessageFile msgFile;
    if (!msgFile.readFile(QString(msgDBFile), messageList)) {
        qWarning() << "Can't load MessageDB:" << msgFile.errorString();
        return;
    }

    if (!messageList.isEmpty()) {
        lastMsgId = messageList.last()->id;
    }
}

void MessageDB::save() const
{
    MessageFile msgFile;
    if (!msgFile.writeFile(QString(msgDBFile), messageList)) {
        qWarning() << "Can't save MessageDB:" << msgFile.errorString();
    }
}

quint32 MessageDB::getNextMsgId()
{
    return ++lastMsgId;
}

void MessageDB::loadHistoryList(MessageRecList *ml) const
{
    QMap <QString, bool> map;
    // Start from the end, load last unique messages
    for (int i = messageList.size() - 1; i >= 0; --i) {
        QString key(messageList.at(i)->chatContact.getPhoneNumber());
        if (!map.contains(key)) {
            map.insert(key,true);
            ml->append(messageList.at(i));
        }
    }
}

void MessageDB::loadViewList(const Cont &contact, MessageRecList *ml) const
{
    QString chatId(contact.getPhoneNumber());
    if (chatId.isEmpty()) {
        return;
    }

    // Save messages with required chat id
    for (int i = 0; i < messageList.size(); ++i) {
        if (messageList.at(i)->chatContact.getPhoneNumber() == chatId) {
            ml->append(messageList.at(i));
        }
    }
}
