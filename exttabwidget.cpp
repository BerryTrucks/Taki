/* $Id: exttabwidget.cpp */
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

#include "exttabwidget.h"
#include <QToolButton>
//#include <QDebug>

ExtTabWidget::ExtTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setParent(parent);
}
QTabBar* ExtTabWidget::tabBar()
{
    return QTabWidget::tabBar();
}

void ExtTabWidget::selectTab()
{
    QToolButton *button = (QToolButton *)sender();
    QString btnName(button->objectName());

    //qDebug() << "tb2 button clicked";

    if (btnName == "tb2") {
        setCurrentIndex(0);
    }
}
