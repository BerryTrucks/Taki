/* $Id: actionmenu.cpp */
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

#include "actionmenu.h"
#include "sysdepapp.h"
#include <QPushButton>
#include <QMouseEvent>
//#include <QDebug>

ActionMenu::ActionMenu(QWidget *parent) :
    QDialog(parent),
    btnName(),
    _startPoint(0,0),
    _endPoint(0,0)
{
}

QString ActionMenu::getButtonName() const
{
    return btnName;
}

void ActionMenu::buttonClicked()
{
    QPushButton *button = (QPushButton *)sender();
    btnName = button->objectName();
    done(QDialog::Accepted);
}

/*
void ActionMenu::focusOutEvent(QFocusEvent *event)
{
    qDebug() << "focusOutEvent in action menu";
    QDialog::focusOutEvent(event);
}
*/

bool ActionMenu::event(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate) {
        done(QDialog::Rejected);
    }
    return QDialog::event(event);
}

void ActionMenu::mousePressEvent(QMouseEvent *event)
{
    _startPoint = event->pos();
    QDialog::mousePressEvent(event);
}

void ActionMenu::mouseReleaseEvent(QMouseEvent *event)
{
    _endPoint = event->pos();

    int xDiff = _startPoint.x() - _endPoint.x();
    int yDiff = _startPoint.y() - _endPoint.y();
    if(qAbs(xDiff) > qAbs(yDiff)) {
        // horizontal swipe detected, now find direction
        if( _startPoint.x() > _endPoint.x() ) {
            //qDebug() << "swipe left";
        } else {
            //qDebug() << "swipe right";
            done(QDialog::Rejected);
            return;
        }
    }
    /*
    else {
        // vertical swipe detected, now find direction
        if( _startPoint.y() > _endPoint.y() ) {
            qDebug() << "swipe up";
        } else {
            qDebug() << "swipe down";
        }
    }
    */
    QDialog::mouseReleaseEvent(event);
}

void ActionMenu::show()
{
    QRect wp = (qobject_cast<SysDepApp *>(qApp))->getActionMenuSize(sizeHint());
    //qDebug() << "min hint" << minimumSizeHint();
    //qDebug() << "size hint" << sizeHint();
    //qDebug() << "app act menu size" << wp;

#if !defined(Q_OS_QNX)
    setWindowFlags(Qt::Popup);
#endif

    QDialog::show();
    setFocus();
    resize(wp.size());
    move(wp.topLeft());
}
