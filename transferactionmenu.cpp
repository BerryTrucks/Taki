/* $Id: transferactionmenu.cpp */
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

#include "transferactionmenu.h"
#include "ui_transferactionmenu.h"
#include <QDebug>

TransferActionMenu::TransferActionMenu(const int selectedCall, QWidget *parent) :
    ActionMenu(parent),
    extension(""),
    ui(new Ui::TransferActionMenu),
    callId(selectedCall),
    btnPressed(false)
{
    ui->setupUi(this);

    connect(this, SIGNAL(controlBtnClick(QString,int,QString)),
            parent, SLOT(transferActionMenuPress(QString,int,QString)));

    if (selectedCall == -1) {
        ui->btAm0->setDisabled(true);
        ui->btAm1->setDisabled(true);
        ui->btAm2->setDisabled(true);
        ui->btAm3->setDisabled(true);
        ui->btAm4->setDisabled(true);
        ui->btAm5->setDisabled(true);
        ui->btAm6->setDisabled(true);
        ui->btAm7->setDisabled(true);
        ui->btAm8->setDisabled(true);
        ui->btAm9->setDisabled(true);
        ui->btAmBlndXfer->setDisabled(true);
        ui->btAmAttnXfer->setDisabled(true);
        ui->btAmCancelXfer->setDisabled(true);
        ui->btAmPound->setDisabled(true);
        ui->btAmStar->setDisabled(true);
    }
    ui->lbTransferExt->setText("");
}

TransferActionMenu::~TransferActionMenu()
{
    delete ui;
}

void TransferActionMenu::digitClick()
{
    QPushButton *button = (QPushButton *)sender();
    if (!button) {
        return;
    }
    QString strKeyId(button->accessibleName());
    bool isOk;
    int keyId = strKeyId.toInt(&isOk, 16);
    if (isOk) {
        extension.append(QChar(keyId));
        ui->lbTransferExt->setText(extension);
    }
}

void TransferActionMenu::onBsBtnClick()
{
    extension.chop(1);
    ui->lbTransferExt->setText(extension);
}

void TransferActionMenu::onControlBtnClick()
{
    QPushButton *button = (QPushButton *)sender();
    if (!button) {
        return;
    }
    QString btnName(button->objectName());
    btnPressed = true;
    //qDebug() << "real control button was pressed";
    emit controlBtnClick(btnName, callId, extension);
    done(QDialog::Accepted);
}

void TransferActionMenu::done(int r)
{
    if (!btnPressed) {
        //qDebug() << "emulate cancel press";
        emit controlBtnClick("btAmCancelXfer", callId, extension);
    }
    ActionMenu::done(r);
}
