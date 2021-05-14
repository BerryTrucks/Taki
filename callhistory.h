/* $Id: callhistory.h */
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

#ifndef CALLHISTORY_H
#define CALLHISTORY_H

#include <QObject>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class Cdr;

class CallHistory : public QObject
{
    Q_OBJECT

public:
    explicit CallHistory(QTreeWidget *, unsigned int numCalls = 0,
                         QObject *parent = 0);
    void addCall(const Cdr &cdr);
    void deleteAll();
    QTreeWidgetItem * prepareItem(const Cdr &cdr);
    void load();
    void save();
    int length();
    void setMaxCalls(const int numCalls);

signals:
    void dialNumber(const QString &number, const QString &dtmf, bool translateNumber);
    void addNewContcat(const QString &name, const QString &number);
    void callDetails(const Cdr &cdr);

public slots:
    int  deleteCurrent();
    void openCallDetails(QTreeWidgetItem * item, int column);
    void onAddCallDetailToContact();
    void addToContact(const QTreeWidgetItem * item);

private:
    QTreeWidget *chTreeWidget;
    int maxCalls;
    void resizeColumns();
    void destroyItem(QTreeWidgetItem *item);
    void deleteItemRecording(const QTreeWidgetItem *item);
};

#endif // CALLHISTORY_H
