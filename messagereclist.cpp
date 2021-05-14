/* $Id: messagereclist.cpp */
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

#include "messagereclist.h"

MessageRecList::~MessageRecList()
{
// Don't clear the list as items may be shared between lists
//    qDeleteAll(*this);
//    clear();
}

MessageRecList::MessageRecList(const MessageRecList &) :
    QList <MessageRec *>()
{
}

MessageRecList& MessageRecList::operator=(const MessageRecList &)
{
    return *this;
}

MessageRec * MessageRecList::findById(quint32 id) const
{
    for (int i=0; i < size(); ++i) {
        if (at(i)->id == id) {
            return at(i);
        }
    }
    return 0;
}

QDebug operator<<(QDebug dbg, const MessageRecList &msgList)
{
    for (int i = 0; i < msgList.size(); ++i) {
        dbg << *msgList.at(i) << "\n";
    }
    return dbg;
}
