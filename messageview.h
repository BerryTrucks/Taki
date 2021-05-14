/* $Id: messageview.h */
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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include "messagerec.h"
#include "messagereclist.h"
#include "QsKineticScroller.h"
#include "htmldelegate.h"
#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class MessageView;
}

class MessageView : public QDialog
{
    Q_OBJECT

public:
    explicit MessageView(const Cont &cont, MessageRecList *viewList,
                         QWidget *parent = 0);
    ~MessageView();
    bool getHasUnreadMsg() const;

protected:
    virtual void resizeEvent(QResizeEvent *ev);

private:
    Ui::MessageView *ui;
#if defined(Q_OS_QNX)
    QsKineticScroller *messageViewScroller;
#endif
    MessageRecList *msgList;
    QStandardItemModel *msgModel;
    HtmlDelegate *delegate;
    Cont chatContact;
    bool hasUnreadMsg;

    void load();
    QList<QStandardItem *> prepareItems(MessageRec *msgNew);
    //QList<QStandardItem *> prepareItems2(MessageRec *msgNew);
    void updateViewFrame();
    void markAllRead();

public slots:
    void onMsgStatus(const MessageRec &msgStatus);
    void onMsgNew(MessageRec *msgNew);
    void onMsgDeleteChat(const Cont &delChatCont);
    void onMsgDeleteAll();

private slots:
    void on_btMsgSend_pressed();
    void on_btMsgSelectContact_clicked();
    void on_btMsgDelete_clicked();

signals:
    void msgSend(MessageRec *msg);
    void msgDeleteChat(const Cont &);
};

#endif // MESSAGEVIEW_H
