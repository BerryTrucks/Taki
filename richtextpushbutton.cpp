/* $Id: ritchtextpushbutton.cpp */
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

#include "richtextpushbutton.h"
#include <QPainter>
#include <QPixmap>
#include <QTextDocument>
#include <QIcon>
#include <QSize>
#include <QMenu>
#include <QStylePainter>
#include <QDebug>

RichTextPushButton::RichTextPushButton(QWidget *parent) :
    QPushButton(parent),
    m_htmlText(),
    isRichText(false)
{
}

RichTextPushButton::RichTextPushButton(const QString & text, QWidget *parent) :
    QPushButton(parent)
{
    setText(text);
}

void RichTextPushButton::setHtml(const QString &text)
{
    m_htmlText = text;
    isRichText = true;

    QPalette palette;
    palette.setBrush(QPalette::ButtonText, Qt::transparent);
    setPalette(palette);

    update();
    updateGeometry();
}

void RichTextPushButton::setText(const QString &text)
{
    QRegExp rx("<.+>.*</.*>");

    if (rx.indexIn(text) != -1) {
        setHtml(text);
    }
    else {
        isRichText = false;
        QPushButton::setText(text);
    }
}

//QString RichTextPushButton::text() const
//{
//    return QString("");
//    return m_htmlText;
    /*
    if (isRichText) {
        QTextDocument richText;
        richText.setHtml(htmlText());
        return richText.toPlainText();
    } else
        return QPushButton::text();
        */
//}

QString RichTextPushButton::htmlText() const
{
    return m_htmlText;
}

void RichTextPushButton::paintEvent(QPaintEvent *event)
{
    /*
    qDebug() << "text " << text();
    qDebug() << "htmlText " << htmlText();
    qDebug() << "parent text " << QPushButton::text();

    if (!QPushButton::text().isEmpty() && (QPushButton::text() != text())) {
        qDebug() << "set text of parent obj to child ";
        qDebug() << "Parent " << QPushButton::text();
        qDebug() << "Child " << text();
        setText(QPushButton::text());
        QPushButton::setText("");
    }
    */

    if (isRichText) {
        QStylePainter p(this);

        QRect buttonRect = rect();
        QPoint point;

        QTextDocument richTextLabel(this->parent());
        richTextLabel.setHtml(htmlText());

        QPixmap richTextPixmap(richTextLabel.size().width(), richTextLabel.size().height());
        richTextPixmap.fill(Qt::transparent);
        QPainter richTextPainter(&richTextPixmap);

        //richTextPainter.setRenderHint(QPainter::TextAntialiasing, false);

        richTextLabel.drawContents(&richTextPainter, richTextPixmap.rect());

        if (!icon().isNull())
            point = QPoint(buttonRect.x() + buttonRect.width() / 2 + iconSize().width() / 2 + 2, buttonRect.y() + buttonRect.height() / 2);
        else
            point = QPoint(buttonRect.x() + buttonRect.width() / 2 - 1, buttonRect.y() + buttonRect.height() / 2);

        buttonRect.translate(point.x() - richTextPixmap.width() / 2, point.y() - richTextPixmap.height() / 2);

        p.drawControl(QStyle::CE_PushButton, getStyleOption());
        p.drawPixmap(buttonRect.left(), buttonRect.top(), richTextPixmap.width(), richTextPixmap.height(),richTextPixmap);
    }
    else {
        QPushButton::paintEvent(event);
    }
}

QStyleOptionButton RichTextPushButton::getStyleOption() const
{
    QStyleOptionButton opt;
    opt.initFrom(this);
    opt.features = QStyleOptionButton::None;
    if (isFlat())
        opt.features |= QStyleOptionButton::Flat;
    if (menu())
        opt.features |= QStyleOptionButton::HasMenu;
    if (autoDefault() || isDefault())
        opt.features |= QStyleOptionButton::AutoDefaultButton;
    if (isDefault())
        opt.features |= QStyleOptionButton::DefaultButton;
    if (isDown() || (menu() && menu()->isVisible()))
        opt.state |= QStyle::State_Sunken;
    if (isChecked())
        opt.state |= QStyle::State_On;
    if (!isFlat() && !isDown())
        opt.state |= QStyle::State_Raised;
    opt.text = text();
    opt.icon = icon();
    opt.iconSize = iconSize();
    return opt;
}
