/* $Id: messagehistory.cpp */
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

#include "messagehistory.h"
#include "ui_messagehistory.h"
#include <QDebug>

MessageHistory::MessageHistory(MessageRecList *histList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageHistory),
#if defined(Q_OS_QNX)
    messageHistoryScroller(new QsKineticScroller(this)),
#endif
    msgList(histList),
    msgModel(new QStandardItemModel(0, 2)),
    delegate(new HtmlDelegate())
{
    ui->setupUi(this);

#if defined(Q_OS_QNX)
    messageHistoryScroller->enableKineticScrollFor(ui->twMsgHistory);
#endif

    load();
    updateHistoryFrame();
}

MessageHistory::~MessageHistory()
{
    delete ui;
    delete delegate;
    delete msgModel;
}

void MessageHistory::updateHistoryFrame()
{
    ui->twMsgHistory->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->twMsgHistory->header()->setResizeMode(1, QHeaderView::ResizeToContents);

    if (msgList->isEmpty()) {
        ui->lbMsgNotFound->show();
        ui->twMsgHistory->hide();
        ui->btMsgDelete->setEnabled(false);
    }
    else {
        ui->lbMsgNotFound->hide();
        ui->twMsgHistory->show();
        ui->btMsgDelete->setEnabled(true);
    }

#if defined(Q_OS_QNX)
#else
    ui->frBBControls->hide();
#endif
}

void MessageHistory::load()
{
    int listSize = msgList->size();
    for (int i = 0; i < listSize; ++i) {
        QList<QStandardItem *> items = prepareItems(msgList->at(i));
        msgModel->appendRow(items);
    }

    ui->twMsgHistory->setModel(msgModel);
    ui->twMsgHistory->setItemDelegate(delegate);
}

void MessageHistory::onChatSelected(const QModelIndex &index)
{
    //qDebug() << "MessageHistory::onChatSelected" << msgList->at(index.row())->chatContact;

    MessageRec *msg = msgList->at(index.row());
    if (msg) {
        msg->setReadStatus(MessageRec::Read);
        if (index.isValid()) {
            QString msgText(msgModel->data(index, Qt::DisplayRole).toString());
            if (!msgText.isEmpty()) {
                msgText.replace("<font color='red'>*</font>","");
                QVariant v(msgText);
                msgModel->setData(index, v, Qt::DisplayRole);
            }
        }

        // Tell MainWindow to open existing MsgView
        emit msgView(msg->chatContact);
    }
}

void MessageHistory::on_btMsgNew_clicked()
{
    // Tell MainWindow to open new MsgView
    Cont cont;
    emit msgView(cont);
}

void MessageHistory::onMsgStatus(const MessageRec &msgStatus)
{
    //qDebug() << "MessageHistory::onMsgStatus";

    // Update history with message status
    int listSize = msgList->size();
    for (int i = 0; i < listSize; ++i) {
        MessageRec *msg = msgList->at(i);
        if (msg->id == msgStatus.id) {
            msg->pjStatusCode = msgStatus.pjStatusCode;
            msg->pjStatusText = msgStatus.pjStatusText;
            msg->deliveryStatus = msgStatus.deliveryStatus;
        }
    }
}

void MessageHistory::onMsgNew(MessageRec *msgNew)
{
    //qDebug() << "MessageHistory::onMsgNew Adding msg to history list";

    // Remove chat if exists
    onMsgDeleteHistoryChat(msgNew->chatContact);

    // Add new item to the top
    QList<QStandardItem *> items = prepareItems(msgNew);
    msgModel->insertRow(0, items);
    msgList->insert(0, msgNew);

    updateHistoryFrame();
    ui->twMsgHistory->scrollToTop();
}

void MessageHistory::onMsgDeleteHistoryChat(const Cont &chatCont)
{
    int listSize = msgList->size();

    // Check it such chat exists
    for (int i = 0; i < listSize; ++i) {
        MessageRec *msg = msgList->at(i);
        // If found, remove existing index from model and list
        if (msg->chatContact.getPhoneNumber() == chatCont.getPhoneNumber()) {
            QList<QStandardItem *> itemsToDel = msgModel->takeRow(i);
            qDeleteAll(itemsToDel);
            msgList->removeAt(i);

            updateHistoryFrame();
            break;
        }
    }
}

QList<QStandardItem *> MessageHistory::prepareItems(MessageRec *msgNew) const
{
    QList<QStandardItem *> items;
    QStandardItem *item0, *item1;
    QString name;

    QString chatName(msgNew->chatContact.getDisplayName());
    if (chatName.isEmpty()) {
        name = msgNew->chatContact.getPhoneNumber();
    }
    else {
        name = chatName;
    }

    QString readFlag("");
    if (msgNew->readStatus == MessageRec::UnRead) {
        readFlag = "<font color='red'>*</font>";
    }

    item0 = new QStandardItem(QString("%1<b>%2</b><br>%3").
                              arg(readFlag).arg(name).arg(msgNew->text));
    item1 = new QStandardItem(QString("%1<br>%2").
                arg(msgNew->timeStamp.toString("MM/dd")).
                arg(msgNew->timeStamp.toString("h:mmap")));
    items << item0 << item1;
    return items;
}

void MessageHistory::on_btMsgDelete_clicked()
{
    msgModel->clear();
    msgList->clear();
    updateHistoryFrame();
    emit msgDeleteAll();
}
