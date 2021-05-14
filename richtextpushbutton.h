/* $Id: ritchtextpushbutton.h */
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

#ifndef RICHTEXTPUSHBUTTON_H
#define RICHTEXTPUSHBUTTON_H

#include <QPushButton>
#include <QString>
#include <QStyleOptionButton>

class RichTextPushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QString htmlText READ htmlText WRITE setHtml)

public:
    explicit RichTextPushButton(QWidget *parent = 0);
    explicit RichTextPushButton(const QString & text, QWidget *parent = 0);

    void setHtml(const QString &text);
    void setText(const QString &text);
    //QString text() const;
    QString htmlText() const;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);

private:
    QString m_htmlText;
    bool isRichText;

    QStyleOptionButton getStyleOption() const;
};

#endif // RICHTEXTPUSHBUTTON_H
