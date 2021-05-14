/* $Id: messageview.cpp */
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

#include "messageview.h"
#include "sysdepapp.h"
#include "ui_messageview.h"
#include <QDebug>

#define NUMCOLUMNS 2

MessageView::MessageView(const Cont &cont, MessageRecList *viewList,
                         QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageView),
#if defined(Q_OS_QNX)
    messageViewScroller(new QsKineticScroller(this)),
#endif
    msgList(viewList),
    msgModel(new QStandardItemModel(0, NUMCOLUMNS)),
    delegate(new HtmlDelegate),
    chatContact(cont),
    hasUnreadMsg(false)
{
    ui->setupUi(this);

#if defined(Q_OS_QNX)
    messageViewScroller->enableKineticScrollFor(ui->twMsgChat);
#endif

    load();
    updateViewFrame();
}

MessageView::~MessageView()
{
    markAllRead();
    delete ui;
    delete delegate;
    delete msgModel;
}

bool MessageView::getHasUnreadMsg() const
{
    return hasUnreadMsg;
}

void MessageView::markAllRead()
{
    if (hasUnreadMsg) {
        // Mark all messages in view as read
        int listSize = msgList->size();
        for (int i = 0; i < listSize; ++i) {
            msgList->at(i)->setReadStatus(MessageRec::Read);
        }
    }
}

void MessageView::updateViewFrame()
{
    // Not supported yet
    ui->btMsgSelectContact->hide();

    if (msgList->isEmpty()) {
        ui->btMsgDelete->setEnabled(false);
    }
    else {
        ui->btMsgDelete->setEnabled(true);
    }

    // New chat
    if (chatContact.getPhoneNumber().isEmpty()) {
        ui->frMsgCont->hide();
        ui->frMsgTo->show();
    }
    else { // Chat with specified phone number
        QString displayName;
        if (chatContact.getDisplayName().isEmpty()) {
            displayName = chatContact.getPhoneNumber();
        }
        else {
            displayName = chatContact.getDisplayName();
        }
        ui->lbContactDisplayName->setText(displayName);
        setWindowTitle(displayName);

        if (chatContact.getPhotoFilepath().isEmpty()) {
            ui->lbContactPhoto->setPixmap(QPixmap(":/icons/contact.png"));
        }
        else {
            ui->lbContactPhoto->setPixmap(QPixmap(chatContact.getPhotoFilepath()));
        }

        if (chatContact.getAction() == Cont::CopyMessage) {
            ui->leMsgTo->setText(chatContact.getPhoneNumber());
            ui->frMsgCont->hide();
            ui->frMsgTo->show();
        }
        else {
            ui->frMsgCont->show();
            ui->frMsgTo->hide();
        }
    }

    ui->twMsgChat->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->twMsgChat->header()->setResizeMode(1, QHeaderView::ResizeToContents);

    ui->twMsgChat->scrollToBottom();

    for(int i = 0; i < NUMCOLUMNS; ++i) {
        ui->twMsgChat->resizeColumnToContents(i);
    }
}

void MessageView::load()
{
    int listSize = msgList->size();

    for (int i = 0; i < listSize; ++i) {
        QList<QStandardItem *> items = prepareItems(msgList->at(i));
        msgModel->appendRow(items);
    }

    ui->twMsgChat->setModel(msgModel);
    ui->twMsgChat->setItemDelegate(delegate);
}

void MessageView::on_btMsgSend_pressed()
{
    if (ui->leMsgEdit->text().isEmpty()) {
        return;
    }

    QString toPhone(ui->leMsgTo->text());
    if (toPhone.isEmpty()) {
        if (chatContact.getPhoneNumber().isEmpty()) {
            return;
        }
        else {
            toPhone = chatContact.getPhoneNumber();
        }
    }
    else {
        chatContact.setPhoneNumber(toPhone);
    }

    // In case it was 'copyMsg' set it to 'message'
    chatContact.setAction(Cont::Message);

    MessageRec *msg = new MessageRec;
    msg->to.setPhoneNumber(toPhone);
    msg->text = ui->leMsgEdit->text();
    msg->chatContact = chatContact;
    msg->setReadStatus(MessageRec::Read);

    //qDebug() << "MessageView Sending messge:" << *msg;
    emit msgSend(msg);
    ui->leMsgEdit->clear();
    ui->leMsgEdit->setFocus();
}

void MessageView::on_btMsgSelectContact_clicked()
{
    // Do nothing on item select
}

void MessageView::on_btMsgDelete_clicked()
{
    if (chatContact.getPhoneNumber().isEmpty()) {
        return;
    }
    emit msgDeleteChat(chatContact);
}

void MessageView::onMsgStatus(const MessageRec &msgStatus)
{
    //qDebug() << "MessageView::onMsgStatus Status received for msg:" << msgStatus;

    int listSize = msgList->size();
    for (int i = 0; i < listSize; ++i) {
        MessageRec *msg = msgList->at(i);
        if (msg->id == msgStatus.id) {
            QList<QStandardItem *> itemsToDel = msgModel->takeRow(i);
            qDeleteAll(itemsToDel);

            QList<QStandardItem *> items = prepareItems(msg);
            msgModel->insertRow(i, items);
        }
    }
}

void MessageView::onMsgNew(MessageRec *msgNew)
{
    // Ignore not our chats
    if (msgNew->chatContact.getPhoneNumber() != chatContact.getPhoneNumber()) {
        //qDebug() << "MessageView::onMsgNew Ignoring message with chatContact"
        //         << msgNew->chatContact << "our chatId is" << chatContact;
        return;
    }

    //qDebug() << "MessageView::onMsgNew Adding msg to View list";

    QList<QStandardItem *> items = prepareItems(msgNew);
    msgModel->appendRow(items);
    msgList->append(msgNew);
    chatContact = msgNew->chatContact;
    updateViewFrame();

    // Possibly set status to Read. If History event delivered after View event then
    // message will be marked as read in history. Not the case now.
    // msgNew->setReadStatus(MessageRec::Read);
}

void MessageView::onMsgDeleteChat(const Cont &delChatCont)
{
    if (delChatCont.getPhoneNumber() != chatContact.getPhoneNumber()) {
        //qDebug() << "MessageView::onMsgDeleteChat Ignoring message with chatCont"
        //         << delChatCont << "our chatCont is" << chatContact;
        return;
    }
    msgModel->clear();
    msgList->clear();
    updateViewFrame();
}

void MessageView::onMsgDeleteAll()
{
    msgModel->clear();
    msgList->clear();
    updateViewFrame();
}
/*
QList<QStandardItem *> MessageView::prepareItems2(MessageRec *msgNew)
{
    QList<QStandardItem *> items;
    QStandardItem *item0, *item1;
    QString itemStr;
    QString name;

    if (msgNew->direction == MessageRec::Out) {
        name = tr("Me");
    }
    else {
        QString chatName(msgNew->chatContact.getDisplayName());
        if (chatName.isEmpty()) {
            name = msgNew->chatId;
        }
        else {
            name = chatName;
        }
    }
    QString status;

    switch (msgNew->deliveryStatus) {
    case MessageRec::New:
        status = tr("New");
        break;
    case MessageRec::Queued:
        status = tr("Queued");
        break;
    case MessageRec::Sent:
        status = tr("Sent");
        break;
    case MessageRec::Failed:
    {
        QString failedTempl("<font color='red'>%1</font>");
        status = failedTempl.arg(tr("Failed"));
        if (!msgNew->pjStatusText.isEmpty()) {
            status.append(QString(" <small>%1</small>").arg(msgNew->pjStatusText));
        }
    }
        break;
    case MessageRec::Delivered:
        status = tr("Delivered");
        break;
    }

    itemStr = QString("<table border='1'>"
                      "<tr><td><b>%1</b></td><td align='right'><small>%4</small></td></tr>"
                      "<tr><td colspan='2'>%2</td></tr>"
                      "<tr><td colspan='2'><small>%3</small></td></tr>"
                      "</table>").
            arg(name).arg(msgNew->text).arg(status).
            arg(msgNew->timeStamp.toString("MM/dd h:mmap"));

    item0 = new QStandardItem(itemStr);
    items << item0;
    return items;
}
*/
QList<QStandardItem *> MessageView::prepareItems(MessageRec *msgNew)
{
    QList<QStandardItem *> items;
    QStandardItem *item0, *item1;
    QString name;

    if (msgNew->direction == MessageRec::Out) {
        name = tr("Me");
    }
    else {
        QString chatName(msgNew->chatContact.getDisplayName());
        if (chatName.isEmpty()) {
            name = msgNew->chatContact.getPhoneNumber();
        }
        else {
            name = chatName;
        }
    }

    QString readFlag("");
    if (msgNew->readStatus == MessageRec::UnRead) {
        readFlag = "<font color='red'>*</font>";
        hasUnreadMsg = true;
    }

    QString itemStr(readFlag);
    itemStr.append(QString("<b>%1</b>").arg(name));
    itemStr.append(QString("<br>%1").arg(msgNew->text));

    QString status(tr("<br><small>%1</small>"));

    switch (msgNew->deliveryStatus) {
    case MessageRec::New:
        itemStr.append(status.arg(tr("New")));
        break;
    case MessageRec::Queued:
        itemStr.append(status.arg(tr("Queued")));
        break;
    case MessageRec::Sent:
        itemStr.append(status.arg(tr("Sent")));
        break;
    case MessageRec::Failed:
    {
        QString faildTempl("<font color='red' size='-1'>%1</font>");
        itemStr.append(status.arg(faildTempl.arg(tr("Failed"))));
        if (!msgNew->pjStatusText.isEmpty()) {
            itemStr.append(QString(" <small>%1</small>").arg(msgNew->pjStatusText));
        }
    }
        break;
    case MessageRec::Delivered:
        itemStr.append(status.arg(tr("Delivered")));
        break;
    }

    item0 = new QStandardItem(itemStr);
    item1 = new QStandardItem(QString("<small>%1<br>%2</small>").
                              arg(msgNew->timeStamp.toString("MM/dd")).
                              arg(msgNew->timeStamp.toString("h:mmap")));
    items << item0 << item1;
    return items;
}

void MessageView::resizeEvent(QResizeEvent *ev)
{
#if defined(Q_OS_QNX)
    SysDepApp::KeyboardChangeStatus kbSt;
    kbSt = (qobject_cast<SysDepApp *>(qApp))->getKeyboardChangeStatus(ev);
    switch (kbSt) {
    case SysDepApp::KbOn:
        ui->frMsgView->hide();
        ui->twMsgChat->scrollToBottom();
        break;
    case SysDepApp::KbOff:
        ui->frMsgView->show();
        ui->twMsgChat->scrollToBottom();
        break;
    default:
        break;
    }
#endif
    QDialog::resizeEvent(ev);
}
