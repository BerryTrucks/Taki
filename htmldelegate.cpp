/* $Id: htmldelegate.cpp */
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

#include "htmldelegate.h"
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QDebug>

void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget ? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setDefaultFont(optionV4.font);
    doc.setTextWidth(optionV4.rect.width());

    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected) {
        ctx.palette.setColor(QPalette::Text,
                             optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));
    }

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);

    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setDefaultFont(optionV4.font);

    int w = optionV4.widget->parentWidget()->width();

#define EXTRAW 80
    if (w > EXTRAW) {
        doc.setTextWidth(w - EXTRAW);
        if (doc.idealWidth() < (w - EXTRAW)) {
            doc.setTextWidth(doc.idealWidth());
        }
    }

    return QSize(doc.idealWidth(), doc.size().height());
}
