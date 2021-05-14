/* $Id: messagereclist.h */
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

#ifndef MESSAGERECLIST_H
#define MESSAGERECLIST_H

#include "messagerec.h"

class MessageRecList : public QList <MessageRec *>
{
public:
    MessageRecList() {}
    ~MessageRecList();
    MessageRec *findById(quint32 id) const;
    friend QDebug operator<<(QDebug dbg, const MessageRecList &msgList);

private:
    MessageRecList(const MessageRecList &);
    MessageRecList& operator=(const MessageRecList &);
};

#endif // MESSAGERECLIST_H
