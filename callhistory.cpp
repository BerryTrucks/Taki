/* $Id: callhistory.cpp */
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

#include "callhistory.h"
#include "cdr.h"
#include "calldetails.h"
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QDebug>

#define HISTORYDELIMITER "~"

#if defined(Q_OS_QNX)
const char *historyFile = "data/callhistory.txt.dat";
#else
const char *historyFile = "callhistory.txt.dat";
#endif


CallHistory::CallHistory(QTreeWidget *tw,
                         unsigned int numCalls,
                         QObject *parent) :
    QObject(parent),
    chTreeWidget(tw),
    maxCalls(numCalls)
{
    load();
    //chTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
    // Stretch the Name/Number column
    chTreeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
    chTreeWidget->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    chTreeWidget->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    //chTreeWidget->header()->setStretchLastSection(false);

    connect(chTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(openCallDetails(QTreeWidgetItem*,int)));
}

void CallHistory::load()
{
    QList<QTreeWidgetItem *> items;

    QString fileName(historyFile);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Error opening CallHistory file";
        return;
    }

    QDataStream in(&file);
    while (!in.atEnd()) {
        Cdr cdr;
        in >> cdr;
        QTreeWidgetItem *twi = prepareItem(cdr);
        items.append(twi);
    }
    file.close();

    chTreeWidget->insertTopLevelItems(0, items);
    resizeColumns();
}

void CallHistory::save()
{
    QString fileName(historyFile);
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qCritical() << "Error opening CallHistory file";
        return;
    }

    QDataStream stream(&file);
    for (int i=0; i < chTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = chTreeWidget->topLevelItem(i);
        // Get stored CDR
        QVariant v = item->data(0, Qt::UserRole);
        Cdr cdr = v.value<Cdr>();
        // Write it
        stream << cdr;
    }
    file.close();
}

void CallHistory::addCall(const Cdr &cdr)
{
    QTreeWidgetItem *twi;
    QList<QTreeWidgetItem *> items;

    if (maxCalls) {
        while ((chTreeWidget->topLevelItemCount() + 1) > maxCalls) {
            twi = chTreeWidget->topLevelItem(chTreeWidget->topLevelItemCount()-1);
            destroyItem(twi);
        }
    }

    twi = prepareItem(cdr);
    items.append(twi);
    chTreeWidget->insertTopLevelItems(0, items);

    // Save list
    save();
    resizeColumns();
}

QTreeWidgetItem * CallHistory::prepareItem(const Cdr &cdr)
{
    QStringList list;
    list << cdr.getName() + "\n" + cdr.getDisplayNumber()
         << cdr.startTime() << "";

    QTreeWidgetItem *newItem = new QTreeWidgetItem((QTreeWidget*)0, list);

    //qDebug() << "cdr disposition " << cdr.getDisposition();

    QString toolTip(
            "Start time: " + cdr.getTmBegin().toString("yyyy-MM-dd hh:mm:ss") + "\n" +
            "Duration: " + cdr.getDuration() + "\n" +
            "Disposition: " + cdr.getDisposition() + "\n" +
            "Last status: " + cdr.getLastStatus() + "\n" +
            "Last status text: " + cdr.getLastStatusText());
    newItem->setToolTip(0, toolTip);
    //newItem->setWhatsThis(0, toolTip);

    if (cdr.getDirection() == "In") {
        if (!cdr.getRecordingPath().isEmpty()) {
            newItem->setIcon(0, QIcon(":icons/ch-inbound-rec.png"));
        }
        else {
            if (cdr.getDisposition() == "OK")
                newItem->setIcon(0, QIcon(":icons/ch-inbound.png"));
            else
                newItem->setIcon(0, QIcon(":icons/ch-inbound-err.png"));
        }
    }
    else {
        if (!cdr.getRecordingPath().isEmpty()) {
            newItem->setIcon(0, QIcon(":icons/ch-outbound-rec.png"));
        }
        else {
            if (cdr.getDisposition() == "OK")
                newItem->setIcon(0, QIcon(":icons/ch-outbound.png"));
            else
                newItem->setIcon(0, QIcon(":icons/ch-outbound-err.png"));
        }
    }

    newItem->setIcon(2, QIcon(":icons/arrow-right.png"));

    // Save CDR record in the list
    QVariant var;
    var.setValue(cdr);
    newItem->setData(0, Qt::UserRole, var);

    return newItem;
}

void CallHistory::resizeColumns()
{
    for(int i = 0; i < chTreeWidget->columnCount(); ++i) {
        //qDebug()<< "resizing column " << i;
        chTreeWidget->resizeColumnToContents(i);
    }
}

int CallHistory::length()
{
    return chTreeWidget->topLevelItemCount();
}

int CallHistory::deleteCurrent()
{
    //qDebug() << "del current call history item";
    QTreeWidgetItem *item = chTreeWidget->currentItem();
    if (!item)
        return 1;

    destroyItem(item);
    save();
    return 0;
}

void CallHistory::destroyItem(QTreeWidgetItem *item)
{
    if (!item)
        return;
    deleteItemRecording(item);
    delete item;
}

void CallHistory::deleteItemRecording(const QTreeWidgetItem *item)
{
    QVariant v = item->data(0, Qt::UserRole);
    Cdr cdr = v.value<Cdr>();
    QString recordingPath(cdr.getRecordingPath());
    if (!recordingPath.isEmpty()) {
        //qDebug() << "removing" << recordingPath;
        QFile::remove(recordingPath);
    }
}

void CallHistory::deleteAll()
{
    QTreeWidgetItemIterator it(chTreeWidget);
    while (*it) {
        deleteItemRecording(*it);
        ++it;
    }
    chTreeWidget->clear();
    save();
}

void CallHistory::setMaxCalls(const int numCalls)
{
    maxCalls = numCalls;
}

void CallHistory::openCallDetails(QTreeWidgetItem * item, int column)
{
    qDebug() << "open call details clicked" << column;
    if (!item)
        return;

    QVariant v = item->data(0, Qt::UserRole);
    Cdr cdr = v.value<Cdr>();

    if (column == 0) {
        QString number(cdr.getNumber());
        emit dialNumber(number, "", false);
    }
    else {
        emit callDetails(cdr);
    }
}

void CallHistory::onAddCallDetailToContact()
{
    //qDebug() << "add to contacts current call history item";
    QTreeWidgetItem *item = chTreeWidget->currentItem();
    addToContact(item);
}

void CallHistory::addToContact(const QTreeWidgetItem *item)
{
    if (!item)
        return;

    QVariant v = item->data(0, Qt::UserRole);
    Cdr cdr = v.value<Cdr>();

    QString name(cdr.getName());
    QString number(cdr.getNumber());

    emit addNewContcat(name, number);
}
