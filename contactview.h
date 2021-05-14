/* $Id: contactview.h */
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

#ifndef CONTACTVIEW_H
#define CONTACTVIEW_H

#include <QTreeWidgetItem>
#include <QDialog>
#include <QMouseEvent>
#include <QPoint>
#include "QsKineticScroller.h"

namespace Ui {
class ContactView;
}

class Cont;

class ContactView : public QDialog
{
    Q_OBJECT

public:
    explicit ContactView(Cont *contact, QWidget *parent = 0);
    ~ContactView();

private:
    Ui::ContactView *ui;
    Cont *m_contact;

    QPoint _startPoint;
    QPoint _endPoint;
    QsKineticScroller *phonesScroller;

protected:
    bool event(QEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void onPhoneSelected(QTreeWidgetItem *item, int column);
    void on_btContDelete_clicked();
    void on_btContEdit_clicked();

signals:
    void editContact();
    void delContact();
};

#endif // CONTACTVIEW_H
