/* $Id: callinfo.cpp */
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

#include "callinfo.h"
#include <pjsua-lib/pjsua.h>

CallInfo::CallInfo(const pjsua_call_info &ci)
{
    c_id = ci.id;
    acc_id = ci.acc_id;
    local_info = QString::fromAscii(ci.local_info.ptr,(int)ci.local_info.slen);
    local_contact = QString::fromAscii(ci.local_contact.ptr,(int)ci.local_contact.slen);
    remote_info = QString::fromAscii(ci.remote_info.ptr,(int)ci.remote_info.slen);
    remote_contact = QString::fromAscii(ci.remote_contact.ptr,(int)ci.remote_contact.slen);
    call_id = QString::fromAscii(ci.call_id.ptr,(int)ci.call_id.slen);
    state = ci.state;
    state_text = QString::fromAscii(ci.state_text.ptr,(int)ci.state_text.slen);
    last_status = QString::number(ci.last_status);
    last_status_text = QString::fromAscii(ci.last_status_text.ptr,(int)ci.last_status_text.slen);
    ring = false;
    transfereeCallId = -1;
}
