/* $Id: cdr.h */
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

#ifndef CDR_H
#define CDR_H

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QMetaType>

class Cdr
{
public:
    Cdr();
    Cdr(const Cdr &other);
    /*
    explicit Cdr(const QDateTime &a_tmBegin, const QDateTime &a_tmAnswer,
        const QDateTime &a_tmEnd,
        const bool a_answered, const bool a_ended, const QString &a_direction,
        const QString &a_last_status, const QString &a_last_status_text,
        const QString &a_disposition, const QString &a_name,
        const QString &a_number, const QString &a_recordingPath);
        */
    ~Cdr();
    void start();
    void answer();
    void end(void);
    void end(const QString &status, const QString &status_text);
    QString durationStr() const;
    QString getDuration() const;
    QString startTime() const;

    QDateTime getTmBegin() const;
    QDateTime getTmAnswer() const;
    QDateTime getTmEnd() const;
    bool isAnswered() const;
    bool isEnded() const;
    QString getDirection() const;
    QString getLastStatus() const;
    QString getLastStatusText() const;
    QString getDisposition() const;
    QString getName() const;
    QString getNumber() const;
    QString getDisplayNumber() const;
    QString getRecordingPath() const;

    void setDirection(const QString &dir);
    void setName(const QString &newName);
    void setNumber(const QString &newNumber);
    void setRecordingPath(const QString &fileName);

    friend QDataStream& operator <<(QDataStream& stream, const Cdr& c);
    friend QDataStream& operator >>(QDataStream& stream, Cdr& c);

private:
    QDateTime tmBegin;
    QDateTime tmAnswer;
    QDateTime tmEnd;
    bool answered;
    bool ended;
    QString direction;
    QString last_status;
    QString last_status_text;
    QString disposition;
    QString name;
    QString number;
    QString recordingPath;
};

Q_DECLARE_METATYPE(Cdr)

#endif // CDR_H
