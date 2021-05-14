/* $Id: bpshandler.h */
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

#ifndef BPSHANDLER_H
#define BPSHANDLER_H

#include <QObject>

struct bps_event_t;

class BpsHandler : public QObject
{
    Q_OBJECT

public:
    explicit BpsHandler(QObject *parent = 0);
    ~BpsHandler();
    void bpsEventHandler(bps_event_t *event);

signals:
    void windowActive();
    void windowInactive();
    void swipeDown();
    void netStatus(bool, int);
    void callBtnClicked(const QString &btnName, int id);

private:
    int m_id;
    bool prevNetAvailable;
    void openConnection();
    void closeConnection();
};

#endif // BPSHANDLER_H
