/* $Id: lineeditwitherase.cpp */
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

#include "lineeditwitherase.h"
#include <QToolButton>
#include <QStyle>

LineEditWithErase::LineEditWithErase(QWidget *parent) :
    QLineEdit(parent)
{
    backspaceButton = new QToolButton(this);

    int frameWidth = frWidth();
    backspaceButton->setIcon(QIcon(":/icons/erase.png"));
    backspaceButton->setIconSize(QSize(sizeHint().height() - 4 * frameWidth,
                                       sizeHint().height() - 4 * frameWidth));
    backspaceButton->setCursor(Qt::ArrowCursor);
    backspaceButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    backspaceButton->setToolTip(tr("Erase"));

    connect(backspaceButton, SIGNAL(clicked()), this, SLOT(onBackSpace()));

    setStyleSheet(QString("LineEdit { padding-right: %1px; }").
                  arg(backspaceButton->sizeHint().width() + frameWidth + 1));

    QSize minSizeHint = minimumSizeHint();
    setMinimumSize(qMax(minSizeHint.width(),
                        backspaceButton->sizeHint().height() + frameWidth),
                   qMax(minSizeHint.height(),
                        backspaceButton->sizeHint().height() + frameWidth));
}

void LineEditWithErase::onBackSpace()
{
    backspace();
}

int LineEditWithErase::frWidth() const
{
    return style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
}

void LineEditWithErase::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QSize size = backspaceButton->sizeHint();
    backspaceButton->move(rect().right() - size.width(),
                          rect().bottom() - size.height() + frWidth());
}
