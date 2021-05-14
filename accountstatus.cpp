/* $Id: accountstatus.cpp */
/*
 * Copyright (C) 2012-2015 Dmytro Mishchenko <arkadiamail@gmail.com>
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

#include "accountstatus.h"
#include "account.h"
#include "ui_accountstatus.h"
#include <QDebug>
#include <QStringList>

AccountStatus::AccountStatus(Account *ac, QWidget *parent) :
    QDialog(parent),
#if defined(Q_OS_QNX)
    infoScroller(new QsKineticScroller()),
#endif
    ui(new Ui::AccountStatus),
    account(ac)
{
    ui->setupUi(this);

#if defined(Q_OS_QNX)
    infoScroller->enableKineticScrollFor(ui->teAccountInfo);
    ui->frDesktopControls->hide();
#else
    ui->frBBControls->hide();
#endif

    if (account) {
        QStringList infoList;

        ui->lbAccount->setText(account->name());

        infoList << "<b>" << tr("Account:") << "</b>" << " "
                 << ((ac->enabled) ? tr("enabled") : tr("disabled"));

        if (ac->sipRegister) {
            if (!ac->regStatus.isEmpty()) {
                infoList << "<br>" << "<b>" << tr("Registration:") << "</b>"
                         << " " << ac->regStatus;
                if (!ac->regStatusText.isEmpty()) {
                    infoList << " " <<  "(" << ac->regStatusText << ")";
                }
            }
            if (ac->regInfo.code > 0) {
                infoList << "<br>" << "<b>" << tr("Last registration code:") << "</b>" << " "
                         << QString::number(ac->regInfo.code)
                         << " " << ac->regInfo.reason;
            }
        }
        else {
            infoList << "<br>" << "<b>" << tr("Registration") << "</b>" << " " << tr("disabled");
        }

        if (ac->mwiStatus) {
            infoList << "<br>" << "<b>" << tr("Voicemail:") << "</b>";
            if (ac->mwiNumNewMsg) {
                infoList << " " << QString::number(ac->mwiNumNewMsg) << " " << tr("new");
            }
            if (ac->mwiNumOldMsg) {
                infoList << " " << QString::number(ac->mwiNumOldMsg) << " " << tr("old");
            }
        }
        else {
            infoList << "<br>" << "<b>" << tr("Voicemail MWI:") << "</b>" << " " << tr("disabled") ;
        }

        if (!ac->regInfo.transportTypeName.isEmpty()) {
            infoList << "<br>" << "<b>" << tr("Transport type:") << "</b>" << " "
                     << ac->regInfo.transportTypeName;
            infoList << "<br>" << "<b>" << tr("Transport info:") << "</b>" << " "
                     << ac->regInfo.transportInfo;

        }

        if (!ac->trInfo.remoteHost.isEmpty()) {
            infoList << "<br>" << "<b>" << tr("Remote host:") << "</b>" << " " << ac->trInfo.remoteHost;
            infoList << "<br>" << "<b>" << tr("Transport state:") << "</b>" << " ";
            switch (ac->trInfo.state) {
            case PJSIP_TP_STATE_CONNECTED:
                infoList << tr("connected");
                break;
            case PJSIP_TP_STATE_DISCONNECTED:
                infoList << tr("disconnected");
                break;
            case PJSIP_TP_STATE_SHUTDOWN:
                infoList << tr("shutdown");
                break;
            case PJSIP_TP_STATE_DESTROY:
                infoList << tr("destroy");
                break;
            default:
                infoList << tr("unknown");
                break;
            }
        }

        if (ac->trInfo.sslInfo) {
            infoList << "<br>" << "<b>" << tr("Secure socket connection:") << "</b>" << " "
                     << ((ac->trInfo.sslEstablished) ? tr("established") : tr("no"));
            infoList << "<br>" << "<b>" << tr("SSL proto:") << "</b>" << " "
                     << ac->trInfo.sslProto.join(" ");
            infoList << "<br>" << "<b>" << tr("SSL cipher name:") << "</b>" << " "
                     << ac->trInfo.sslCipherName;
            infoList << "<br>" << "<b>" << tr("SSL verification:") << "</b>" << " "
                     << ((ac->trInfo.sslVerifyStatus == 0) ?
                             "<font color=\"green\">" : "<font color=\"red\">")
                     << ac->trInfo.sslVerifyStrings.join(" ") << "</font>";
            infoList << "<br>" << "<b>" << tr("SSL remote certificate:") << "</b>" << " "
                     << "<pre style=\"white-space: pre-wrap; font-size:small\">"
                     << ac->trInfo.sslRemoteCertString << "</pre>";
            if (!ac->trInfo.sslLocalCert.subjectCn.isEmpty() ||
                !ac->trInfo.sslLocalCert.subjectInfo.isEmpty())
            {
                infoList << "<br>" << "<b>" << tr("SSL local certificate:") << "</b>" << " "
                         << "<pre style=\"white-space: pre-wrap; font-size:small\">"
                         << ac->trInfo.sslLocalCertString << "</pre>";
            }
        }

        /*
        QString acInfoStr;
        QDebug  qaci(&acInfoStr);
        qaci << *ac << ac->trInfo;
        infoList << QString("<pre>") + acInfoStr + QString("</pre>");
        */

        ui->teAccountInfo->setHtml(infoList.join(""));
    }
}

AccountStatus::~AccountStatus()
{
#if defined(Q_OS_QNX)
    delete infoScroller;
#endif
    delete ui;
}
