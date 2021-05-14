/* $Id: calldetails.cpp */
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

#include "calldetails.h"
#include "sysdepapp.h"
#include "ui_calldetails.h"
#include <QDebug>


CallDetails::CallDetails(const Cdr &cdr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallDetails),
    m_cdr(cdr)
{
    ui->setupUi(this);

    QString toolTip(
            tr("Name: ") + cdr.getName() + "\n" +
            tr("Number: ") + cdr.getDisplayNumber() + "\n" +
            tr("Start time: ") + cdr.getTmBegin().toString("yyyy-MM-dd hh:mm:ss") + "\n" +
            tr("End time: ") + cdr.getTmEnd().toString("yyyy-MM-dd hh:mm:ss") + "\n" +
            tr("Duration: ") + cdr.getDuration() + "\n" +
            tr("Disposition: ") + cdr.getDisposition() + "\n" +
            tr("Last status: ") + cdr.getLastStatus() + "\n" +
            tr("Last status text: ") + cdr.getLastStatusText());
    if (!cdr.getRecordingPath().isEmpty()) {
        toolTip.append("\n");
        toolTip.append(tr("Call with recording"));
    }

    ui->callDetailsText->setText(toolTip);

    QString rec_path(m_cdr.getRecordingPath());
    if (rec_path.isEmpty()) {
        ui->btCallDetailsPlay->hide();
        ui->btCallDetailsStop->hide();
        ui->btCallDetailsPlayBB->hide();
        ui->btCallDetailsStopBB->hide();
    }
    else {
    }

#if defined(Q_OS_QNX)
    ui->frDesktopControls->hide();
    ui->btCallDetailsCloseBB->hide();
#else
    ui->frBBControls->hide();
#endif
}

CallDetails::~CallDetails()
{
    stopPlayback();
    delete ui;
}

void CallDetails::startPlayback()
{
    QString fn(m_cdr.getRecordingPath());
    if (fn.isEmpty()) {
        return;
    }

    emit startWavPlayback(fn);
}

void CallDetails::stopPlayback()
{
    emit stopWavPlayback();
}

void CallDetails::on_btCallDetailsDelete_clicked()
{
    //qDebug() << "Del pressed";
    done(QDialog::Accepted);
    emit delCallDetail();
}

void CallDetails::on_btCallDetailsAddContact_clicked()
{
    //qDebug() << "Add pressed";
    done(QDialog::Accepted);
    emit addCallDetailToContact();
}
