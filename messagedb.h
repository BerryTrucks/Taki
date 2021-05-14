/* $Id: messagedb.h */
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

#ifndef MESSAGEDB_H
#define MESSAGEDB_H

#include "messagereclist.h"
#include <QObject>

class MessageDB : public QObject
{
    Q_OBJECT

public:
    explicit MessageDB(QObject *parent = 0);
    ~MessageDB();

    quint32 getNextMsgId();
    void loadHistoryList(MessageRecList *ml) const;
    void loadViewList(const Cont &contact, MessageRecList *ml) const;

private:
    quint32 lastMsgId;
    MessageRecList messageList;

    void load();
    void save() const;

public slots:
    void onMsgStatus(const MessageRec &msgStatus);
    void onMsgNew(MessageRec *msg);
    void onMsgDeleteChat(const Cont &chatContact);
    void onMsgDeleteAll();
    void onMsgUpdated();
};

#endif // MESSAGEDB_H
