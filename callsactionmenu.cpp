/* $Id: callsactionmenu.cpp */
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
#include "callsactionmenu.h"
#include "ui_callsactionmenu.h"

CallsActionMenu::CallsActionMenu(const int selectedCall, QWidget *parent) :
    ActionMenu(parent),
    ui(new Ui::CallsActionMenu)
{
    ui->setupUi(this);

    connect(ui->btAmHangup, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm1, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm2, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm3, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm4, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm5, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm6, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm7, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm8, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm9, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAm0, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAmStar, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));
    connect(ui->btAmPound, SIGNAL(clicked()), parent, SLOT(callsActionMenuPress()));

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
        ui->btAmHangup->setDisabled(true);
        ui->btAmPound->setDisabled(true);
        ui->btAmStar->setDisabled(true);
    }
    ui->lbNoActCalls->hide();
}

CallsActionMenu::~CallsActionMenu()
{
    delete ui;
}
