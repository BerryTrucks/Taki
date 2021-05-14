/* $Id: accountnewdialog.cpp */
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

#include "accountnewdialog.h"
#include "ui_accountnewdialog.h"
#include <QDebug>

AccountNewDialog::AccountNewDialog(Account *account, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AccountNewDialog),
#if defined(Q_OS_QNX)
    settingsScroller(new QsKineticScroller(this)),
#endif
    ac(account)
{
    ui->setupUi(this);
#if defined(Q_OS_QNX)
    ui->frDesktopControls->hide();
#else
    ui->frBBControls->hide();
#endif
#if defined(Q_OS_QNX)
    settingsScroller->enableKineticScrollFor(ui->saSettingsUser);
#endif
}

AccountNewDialog::~AccountNewDialog()
{
    delete ui;
}

void AccountNewDialog::accept()
{
    // Save dialog data on Accept
    ac->displayName = ui->leDisplayName->text();
    ac->domain = ui->leDomain->text();
    ac->userPassword = ui->lePassword->text();
    ac->userId = ui->leUserId->text();
    ac->sipRegister = ui->registerCheckBox->checkState();

    QDialog::accept();
}
