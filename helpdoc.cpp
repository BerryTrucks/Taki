/* $Id: helpdoc.cpp */
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

#include "helpdoc.h"
#include "ui_helpdoc.h"


HelpDoc::HelpDoc(QWidget *parent) :
    QDialog(parent),
#if defined(Q_OS_QNX)
    helpScroller(new QsKineticScroller()),
#endif
    ui(new Ui::HelpDoc)
{
    ui->setupUi(this);

#if defined(Q_OS_QNX)
    helpScroller->enableKineticScrollFor(ui->manBrowser);
    ui->frDesktopControls->hide();
#else
    ui->frBBControls->hide();
#endif
}

HelpDoc::~HelpDoc()
{
#if defined(Q_OS_QNX)
    delete helpScroller;
#endif
    delete ui;
}

void HelpDoc::onBackwardAvailable(bool available)
{
    ui->btBackBB->setEnabled(available);
    ui->btBack->setEnabled(available);
}
