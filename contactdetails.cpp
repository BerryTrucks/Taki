/* $Id: contactdetails.cpp */
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

#include "contactdetails.h"
#include "ui_contactdetails.h"
#include <QDebug>

ContactDetails::ContactDetails(Cont &cont, bool isNew, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDetails),
    m_cont(&cont),
#if defined(Q_OS_QNX)
    contactDetailsScroller(new QsKineticScroller(this)),
#endif
    phoneEditControls()
{
    ui->setupUi(this);

    ui->leFirstName->setText(cont.getFirstName());
    ui->leLastName->setText(cont.getLastName());
    ui->leCompany->setText(cont.getCompany());
    ui->lePhone->setText(cont.getPhoneNumber());

#ifdef BB10_BLD
    const QList<ContactAttribute> phones = cont.getPhones();
    if (!phones.isEmpty()) {
        ui->lbPhone->hide();
        ui->lePhone->hide();

        for (int i = 0; i < phones.size(); ++i) {
            ExtLineEdit *le = new ExtLineEdit(this);
            phoneEditControls.append(le);
            QString labelName(phoneType(phones.at(i).subKind()) + tr(" Phone:"));
            le->setText(phones.at(i).value());
            ui->layContactDetails->addRow(labelName, le);
        }
    }
#endif

    if (isNew)
        ui->bbFrameLabel->setText(tr("New Contact"));

#if defined(Q_OS_QNX)
    ui->frDesktopControls->hide();
#else
    ui->frBBControls->hide();
#endif

#if defined(Q_OS_QNX)
    contactDetailsScroller->enableKineticScrollFor(ui->saContactDetails);
#endif
}

ContactDetails::~ContactDetails()
{
    delete ui;
}

void ContactDetails::accept()
{
    if (ui->leFirstName->text().isEmpty()
            && ui->leLastName->text().isEmpty()
            && ui->leCompany->text().isEmpty() )
    {
        done(QDialog::Rejected);
        return;
    }

    m_cont->setFirstName(ui->leFirstName->text());
    m_cont->setLastName(ui->leLastName->text());
    m_cont->setCompany(ui->leCompany->text());
    m_cont->setPhoneNumber(ui->lePhone->text());

    for(int i = 0; i < phoneEditControls.size(); ++i) {
        QLineEdit *le = phoneEditControls.at(i);
        m_cont->setPhoneNumberAt(i, le->text());
    }
    done(QDialog::Accepted);
}

void ContactDetails::reject()
{
    done(QDialog::Rejected);
}
