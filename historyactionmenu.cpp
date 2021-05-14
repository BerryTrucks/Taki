/* $Id: historyactionmenu.cpp */
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

#include "historyactionmenu.h"
#include "ui_historyactionmenu.h"

HistoryActionMenu::HistoryActionMenu(const bool historyLength, const bool showPlayer,
                                     QWidget *parent) :
    ActionMenu(parent),
    ui(new Ui::HistoryActionMenu)
{
    ui->setupUi(this);

    connect(ui->btCall, SIGNAL(clicked()), parent, SLOT(callHistoryDial()));
    connect(ui->btDetails, SIGNAL(clicked()), parent, SLOT(callHistoryDetails()));
    if (showPlayer) {
        connect(ui->btPlay, SIGNAL(clicked()), parent, SLOT(callHistoryStartPlayback()));
        connect(ui->btStop, SIGNAL(clicked()), parent, SLOT(callHistoryStopPlayback()));
    }
    else {
        /*
        ui->btPlay->hide();
        ui->btStop->hide();
        */
        ui->btPlay->setDisabled(true);
        ui->btStop->setDisabled(true);
    }
    connect(ui->btAddContact, SIGNAL(clicked()), parent, SLOT(callHistoryAddContact()));
    connect(ui->btHistorySettings, SIGNAL(clicked()), parent, SLOT(callHistorySettings()));
    connect(ui->btDelete, SIGNAL(clicked()), parent, SLOT(callHistoryDeleteItem()));
    connect(ui->btDeleteAll, SIGNAL(clicked()), parent, SLOT(callHistoryClear()));

    if (!historyLength) {
        /*
        ui->btAddContact->hide();
        ui->btCall->hide();
        ui->btDelete->hide();
        ui->btDeleteAll->hide();
        ui->btDetails->hide();
        ui->btPlay->hide();
        ui->btStop->hide();
*/
        ui->btAddContact->setDisabled(true);
        ui->btCall->setDisabled(true);
        ui->btDelete->setDisabled(true);
        ui->btDeleteAll->setDisabled(true);
        ui->btDetails->setDisabled(true);
        ui->btPlay->setDisabled(true);
        ui->btStop->setDisabled(true);
    }

    // Buttons should be removed. Hide now
    ui->btCall->hide();
    ui->btPlay->hide();
    ui->btStop->hide();
    ui->btDetails->hide();
    ui->btAddContact->hide();
    ui->btDelete->hide();
}

HistoryActionMenu::~HistoryActionMenu()
{
    delete ui;
}
