/* $Id: messagefile.h */
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

#ifndef MESSAGEFILE_H
#define MESSAGEFILE_H

#include "messagereclist.h"
#include <QByteArray>

class MessageFile
{
public:
    MessageFile();

    bool writeFile(QIODevice* ioDevice, const MessageRecList &messageRecList);
    bool readFile(QIODevice* ioDevice, MessageRecList &messageRecList);

    bool writeFile(const QString& filePath, const MessageRecList &messageRecList);
    bool readFile(const QString& filePath, MessageRecList &messageRecList);

    bool writeFile(const QByteArray& byteArray, const MessageRecList &messageRecList);
    bool readFile(QByteArray& byteArray, MessageRecList &messageRecList);

    const QString& errorString( void ) const { return m_errorString; }

 private:
     QString m_errorString;

     static const QByteArray fileHeaderByteArray;
     static const quint16 fileVersion;
};

#endif // MESSAGEFILE_H
