/* $Id: contactview.cpp */
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

#include "cont.h"
#include "contactview.h"
#include "sysdepapp.h"
#include "ui_contactview.h"
#include <QDebug>

ContactView::ContactView(Cont *contact, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactView),
    m_contact(contact),
    _startPoint(0,0),
    _endPoint(0,0),
    phonesScroller(new QsKineticScroller(this))
{
    ui->setupUi(this);

    ui->lbContactDisplayName->setText(contact->getDisplayName());
    if (contact->getPhotoFilepath().isEmpty()) {
        ui->lbContactPhoto->setPixmap(QPixmap(":/icons/contact.png"));
    }
    else {
        ui->lbContactPhoto->setPixmap(QPixmap(contact->getPhotoFilepath()));
    }

    QList<QTreeWidgetItem *> items;
    const QList<ContactAttribute> phones = contact->getPhones();
    for (int i = 0; i < phones.size(); ++i) {
        QStringList listCall;
        listCall << tr("Call") + " " + phoneType(phones.at(i).subKind()) + "   "
                 << phones.at(i).value()
                 << ""
                 << QString::number(Cont::Dial);
        QTreeWidgetItem *newItemCall = new QTreeWidgetItem((QTreeWidget*)0, listCall);
        newItemCall->setIcon(2, QIcon(":icons/copy.png"));
        items.append(newItemCall);

        QStringList listMsg;
        listMsg << tr("Message") + " " + phoneType(phones.at(i).subKind()) + "   "
                << phones.at(i).value()
                << ""
                << QString::number(Cont::Message);
        QTreeWidgetItem *newItemMsg = new QTreeWidgetItem((QTreeWidget*)0, listMsg);
        newItemMsg->setIcon(2, QIcon(":icons/copy.png"));
        items.append(newItemMsg);
    }

    if (!items.isEmpty())
        ui->twContPhones->insertTopLevelItems(0, items);

    ui->twContPhones->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->twContPhones->header()->setResizeMode(1, QHeaderView::Stretch);
    ui->twContPhones->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    ui->twContPhones->header()->setSectionHidden(3, true);
    //ui->twContPhones->header()->setStretchLastSection(false);

    //for(int i = 0; i < 3; ++i)
    //       ui->twContPhones->resizeColumnToContents(i);

    if (phones.size() > 6) {
        // Assume contacts will take bigger part of the screen
        phonesScroller->enableKineticScrollFor(ui->twContPhones);
    }
    else {
        // Filter events to react on right swipe to close the window
        ui->twContPhones->viewport()->installEventFilter(this);
    }
}

ContactView::~ContactView()
{
    delete ui;
}

void ContactView::onPhoneSelected(QTreeWidgetItem *item, int column)
{
    /*qDebug() << "Selected item. Type:" << item->text(0)
             << "number:" << item->text(1)
             << "Column:" << column
             << "action" << static_cast<Cont::ContAction>(item->text(3).toInt());*/

    const QList<ContactAttribute> phones = m_contact->getPhones();
    for (int i = 0; i < phones.size(); ++i) {
        if (phones.at(i).value() == item->text(1)) {
            m_contact->setPhoneNumber(phones.at(i).value());
            m_contact->setPhoneNumberType(phones.at(i).subKind());

            Cont::ContAction act = static_cast<Cont::ContAction>(item->text(3).toInt());
            if (column == 2) {
                if (act == Cont::Message) {
                    m_contact->setAction(Cont::CopyMessage);
                }
                else {
                    m_contact->setAction(Cont::Copy);
                }
            }
            else {
                if (act == Cont::Message) {
                    m_contact->setAction(Cont::Message);
                }
                else {
                    m_contact->setAction(Cont::Dial);
                }
            }
            break;
        }
    }
    done(QDialog::Accepted);
}

void ContactView::on_btContDelete_clicked()
{
    //qDebug() << "Del pressed";
    done(QDialog::Rejected);
    emit delContact();
}

void ContactView::on_btContEdit_clicked()
{
    //qDebug() << "Edit pressed";
    done(QDialog::Rejected);
    emit editContact();
}

bool ContactView::event(QEvent *event)
{
    //qDebug() << "event" << event;
    if (event->type() == QEvent::WindowDeactivate) {
        done(QDialog::Rejected);
    }
    return QDialog::event(event);
}

void ContactView::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "mousePressEvent event" << event;
    _startPoint = event->pos();
    QDialog::mousePressEvent(event);
}

void ContactView::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "mouseReleaseEvent event" << event;

    const int START_X_SWIPE = 100;
    _endPoint = event->pos();

    int xDiff = _startPoint.x() - _endPoint.x();
    int yDiff = _startPoint.y() - _endPoint.y();

    //qDebug() << "Diffs:" << "xDiff" << xDiff << "yDiff" << yDiff;

    if(qAbs(xDiff) > qAbs(yDiff) && qAbs(xDiff) > START_X_SWIPE)
    {
        // horizontal swipe detected, now find direction
        if(_startPoint.x() > _endPoint.x()) {
            //qDebug() << "swipe left" << _startPoint.x() << _endPoint.x();
        } else {
            //qDebug() << "swipe right" << _startPoint.x() << _endPoint.x();
            done(QDialog::Rejected);
            return;
        }
    }
    QDialog::mouseReleaseEvent(event);
}

bool ContactView::eventFilter(QObject *target, QEvent *event)
{
    // Check if its a mouse action in TreeWidget constrol
    if (target == ui->twContPhones->viewport()) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            mousePressEvent(me);
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            mouseReleaseEvent(me);
        }
    }
    return QDialog::eventFilter(target, event);
}
