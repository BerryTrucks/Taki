/* $Id: extlineedit.h */
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

#ifndef EXTLINEEDIT_H
#define EXTLINEEDIT_H

#include <QLineEdit>

class QTimer;
class QMenu;

class ExtLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit ExtLineEdit(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

private:
    QTimer *m_timer;
    bool m_timerTimeout;

    QMenu* createStandardContextMenu();
    void showContextMenu();
    bool isLetter(QKeyEvent *e);
    bool m_autoRepeat;
    bool m_charReplaced;
    int m_lastKey;

private slots:
    void onTimerTimeout();
};

#endif // EXTLINEEDIT_H
