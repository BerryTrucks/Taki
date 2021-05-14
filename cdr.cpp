/* $Id: cdr.cpp */
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

#include "cdr.h"
#include <QRegExp>
//#include <QDebug>

Cdr::Cdr() :
    answered(false),
    ended(false),
    disposition("")
{
    start();
}

Cdr::Cdr(const Cdr &other)
{
     tmBegin = other.tmBegin;
     tmAnswer = other.tmAnswer;
     tmEnd = other.tmEnd;
     answered = other.answered;
     ended = other.ended;
     direction = other.direction;
     last_status = other.last_status;
     last_status_text = other.last_status_text;
     disposition = other.disposition;
     name = other.name;
     number = other.number;
     recordingPath = other.recordingPath;
}
/*
Cdr::Cdr(const QDateTime &a_tmBegin, const QDateTime &a_tmAnswer,
         const QDateTime &a_tmEnd,
         const bool a_answered, const bool a_ended, const QString &a_direction,
         const QString &a_last_status, const QString &a_last_status_text,
         const QString &a_disposition, const QString &a_name,
         const QString &a_number, const QString &a_recordingPath) :
    tmBegin(a_tmBegin),
    tmAnswer(a_tmAnswer),
    tmEnd(a_tmEnd),
    answered(a_answered),
    ended(a_ended),
    direction(a_direction),
    last_status(a_last_status),
    last_status_text(a_last_status_text),
    disposition(a_disposition),
    name(a_name),
    number(a_number),
    recordingPath(a_recordingPath)
{
}*/

Cdr::~Cdr()
{
    if (!ended)
        end();
}

void Cdr::start()
{
    tmBegin = QDateTime::currentDateTime();
}

void Cdr::answer()
{
    if (answered)
        return;
    tmAnswer = QDateTime::currentDateTime();
    answered = true;
    disposition = "OK";
}

void Cdr::end()
{
    if (ended)
        return;
    tmEnd = QDateTime::currentDateTime();
    ended = true;
    if (disposition != "OK") {
        disposition = "Unanswered";
    }
}

void Cdr::end(const QString &status, const QString &status_text)
{
    end();
    last_status = status;
    last_status_text = status_text;
}

QString Cdr::durationStr() const
{
    int seconds = 0;
    if (answered) {
        QDateTime tm;
        tm = (ended) ? tmEnd : QDateTime::currentDateTime();
        seconds = tmAnswer.secsTo(tm);
    }
    QTime t1,t2;
    t2 = t1.addSecs(seconds);
    QString dur(t2.toString("H:mm:ss"));
    return dur;
}

QString Cdr::startTime() const
{
//    QString st(tmBegin.toString("yyyy-MM-dd hh:mm:ss"));
    return tmBegin.toString("MM/dd\nh:mmap");
}

QString Cdr::getDuration() const
{
    return durationStr();
}

QDateTime Cdr::getTmBegin() const
{
    return tmBegin;
}

QDateTime Cdr::getTmAnswer() const
{
    return tmAnswer;
}

QDateTime Cdr::getTmEnd() const
{
    return tmEnd;
}

bool Cdr::isAnswered() const
{
    return answered;
}

bool Cdr::isEnded() const
{
    return ended;
}

QString Cdr::getDirection() const
{
    return direction;
}

QString Cdr::getLastStatus() const
{
    return last_status;
}

QString Cdr::getLastStatusText() const
{
    return last_status_text;
}

QString Cdr::getDisposition() const
{
    return disposition;
}

QString Cdr::getName() const
{
    return name;
}

QString Cdr::getNumber() const
{
    return number;
}

QString Cdr::getDisplayNumber() const
{
    QString ns(number);

    // Check if it has numbers only
    QRegExp rx("\\D");
    if(ns.contains(rx)) {
        // Contains non digits. Leave as is.
        return ns;
    }
    int len = ns.length();
    if (!len)
        return ns;

/*  old formating
    if (len > 5) {
        ns.insert(len-4, "-");
        if (len > 7 ) {
            if (len > 9) {
                ns.insert(len-(7), ") ");
                ns.insert(len-(10), "(");
                if (len > 10)
                    ns.insert(len-(10), " ");
            }
            else
                ns.insert(len-(7), "-");
        }
    }
*/
    /*
      1234
      1 234 5
      1 234 5323
      1 234 532 33
      1 234 532 3333333
      023232323
      234
      234 4
      234 4323
      234 432 33
      234 432 332323
      how to format UK numbers:  http://www.area-codes.org.uk/formatting.php
    */
    if (ns.startsWith(QChar('0'))) {
        return ns;
    }
    else if (ns.startsWith(QChar('1'))) {
        if (len <= 4)
            return ns;
        ns.insert(1, " ");
        ns.insert(5, " ");
        if (len > 8)
            ns.insert(9, " ");
    }
    else {
        if (len < 4) {
            return ns;
        }
        if (len <= 7) {
            ns.insert(3, " ");
            return ns;
        }
        else {
            ns.insert(3, " ");
            ns.insert(7, " ");
        }
    }
    return ns;
}

QString Cdr::getRecordingPath() const
{
    return recordingPath;
}

void Cdr::setDirection(const QString &dir)
{
    direction = dir;
}

void  Cdr::setName(const QString &newName)
{
    name = newName;
}

void Cdr::setNumber(const QString &newNumber)
{
    number = newNumber;
}

void Cdr::setRecordingPath(const QString &fileName)
{
    recordingPath = fileName;
}

QDataStream &operator<<(QDataStream &out, const Cdr &c)
{
    return out << c.tmBegin << c.tmAnswer << c.tmEnd
               << c.answered << c.ended << c.direction
               << c.last_status << c.last_status_text
               << c.disposition << c.name << c.number
               << c.recordingPath;
}

QDataStream &operator>>(QDataStream &in, Cdr &c)
{
    return in >> c.tmBegin >> c.tmAnswer >> c.tmEnd
               >> c.answered >> c.ended >> c.direction
               >> c.last_status >> c.last_status_text
               >> c.disposition >> c.name >> c.number
               >> c.recordingPath;
}
