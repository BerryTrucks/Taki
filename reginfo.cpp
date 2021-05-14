/* $Id: reginfo.cpp */
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

#include "reginfo.h"
#include <QDebug>

RegInfo::RegInfo() :
    accId(0),
    transportFlag(0),
    status(0),
    code(0),
    renew(0)
{
}

RegInfo::RegInfo(int acc, pjsua_reg_info *info) :
    accId(0),
    transportFlag(0),
    status(0),
    code(0),
    renew(0)
{
    pjsip_regc_info regcinfo;
    pjsip_regc_get_info(info->regc, &regcinfo);

    accId = acc;
    serverUri = QString::fromAscii(regcinfo.server_uri.ptr, (int)regcinfo.server_uri.slen);

    if (regcinfo.transport) {
        transportObjName = regcinfo.transport->obj_name;
        transportTypeName = regcinfo.transport->type_name;
        transportInfo = regcinfo.transport->info;
        transportFlag = regcinfo.transport->flag;
    }

    if (info->cbparam) {
        status = info->cbparam->status;
        code = info->cbparam->code;
        reason = QString::fromAscii((char *)info->cbparam->reason.ptr,
                                    info->cbparam->reason.slen);
        renew = info->renew;
    }
}

QDebug operator<<(QDebug dbg, const RegInfo &ri)
{
    dbg.maybeSpace() << "Account Id:" << ri.accId << endl;
    dbg.maybeSpace() << "Server URI:" << ri.serverUri << endl;
    dbg.maybeSpace() << "Transport object name:" << ri.transportObjName << endl;
    dbg.maybeSpace() << "Transport type name:" << ri.transportTypeName << endl;
    dbg.maybeSpace() << "Transport info:" << ri.transportInfo << endl;
    dbg.maybeSpace() << "Transport flag:" << ri.transportFlag << endl;
    dbg.maybeSpace() << "Status:" << ri.status << endl;
    dbg.maybeSpace() << "Code:" << ri.code << endl;
    dbg.maybeSpace() << "Reason:" << ri.reason << endl;
    dbg.maybeSpace() << "Renew:" << ri.renew << endl;
    return dbg.maybeSpace();
}
