/* $Id: messagehistory.h */
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

#ifndef MESSAGEHISTORY_H
#define MESSAGEHISTORY_H

#include "cont.h"
#include "messagerec.h"
#include "messagereclist.h"
#include "QsKineticScroller.h"
#include "htmldelegate.h"
#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class MessageHistory;
}

class MessageHistory : public QDialog
{
    Q_OBJECT

public:
    explicit MessageHistory(MessageRecList *histList, QWidget *parent = 0);
    ~MessageHistory();

private:
    Ui::MessageHistory *ui;
#if defined(Q_OS_QNX)
    QsKineticScroller *messageHistoryScroller;
#endif
    MessageRecList *msgList;
    QStandardItemModel *msgModel;
    HtmlDelegate* delegate;

    void updateHistoryFrame();
    void load();
    void reload();
    QList<QStandardItem *> prepareItems(MessageRec *msgNew) const;

public slots:
    void onMsgStatus(const MessageRec &msgStatus);
    void onMsgNew(MessageRec *msgNew);
    void onMsgDeleteHistoryChat(const Cont &chatCont);

private slots:
    void onChatSelected(const QModelIndex & index);
    void on_btMsgNew_clicked();
    void on_btMsgDelete_clicked();

signals:
    void msgView(const Cont &);
    void msgDeleteAll();
};

#endif // MESSAGEHISTORY_H
