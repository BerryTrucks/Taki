/* $Id: callinfo.h */
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

#ifndef CALLINFO_H
#define CALLINFO_H

#include <QObject>
#include <QMetaType>

struct pjsua_call_info;

class CallInfo {
public:
    CallInfo() {}
    explicit CallInfo(const pjsua_call_info &ci);
    ~CallInfo() {}

    int c_id;
    int acc_id;
    QString local_info;         /* to */
    QString local_contact;
    QString remote_info;        /* from */
    QString remote_contact;
    QString call_id;
    int state;
    QString state_text;
    QString last_status;
    QString last_status_text;

    bool ring;
    int transfereeCallId;
};

Q_DECLARE_METATYPE(CallInfo)

#endif // CALLINFO_H
