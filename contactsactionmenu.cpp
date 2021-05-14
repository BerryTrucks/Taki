/* $Id: contactsactionmenu.cpp */
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

#include "contactsactionmenu.h"
#include "ui_contactsactionmenu.h"

ContactsActionMenu::ContactsActionMenu(const int contactsLength, QWidget *parent) :
    ActionMenu(parent),
    ui(new Ui::ContactsActionMenu)
{
    ui->setupUi(this);

    connect(ui->btCall, SIGNAL(clicked()), parent, SLOT(addressBookDial()));
    connect(ui->btMsg, SIGNAL(clicked()), parent, SLOT(addressBookMsg()));
    connect(ui->btAdd, SIGNAL(clicked()), parent, SLOT(addressBookAddItem()));
    connect(ui->btEdit, SIGNAL(clicked()), parent, SLOT(addressBookEditItem()));
    connect(ui->btDelete, SIGNAL(clicked()), parent, SLOT(addressBookDeleteItem()));

    if (!contactsLength) {
        /*
        ui->btCall->hide();
        ui->btDelete->hide();
        ui->btEdit->hide();
        */
        ui->btCall->setDisabled(true);
        ui->btDelete->setDisabled(true);
        ui->btEdit->setDisabled(true);
    }
}

ContactsActionMenu::~ContactsActionMenu()
{
    delete ui;
}
