/* $Id: contactdetails.h */
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

#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "cont.h"
#include "QsKineticScroller.h"
#include <QDialog>
#include <QLineEdit>
#include <QList>

namespace Ui {
class ContactDetails;
}

class ContactDetails : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDetails(Cont &cont, bool isNew = true, QWidget *parent = 0);
    ~ContactDetails();

private slots:
    void accept();
    void reject();

private:
    Ui::ContactDetails *ui;
    Cont *m_cont;
    QsKineticScroller *contactDetailsScroller;
    QList <QLineEdit *> phoneEditControls;
};

#endif // CONTACTDETAILS_H
