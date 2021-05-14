/* $Id: messagefile.cpp */
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

#include "messagefile.h"
#include <QDataStream>
#include <QFileInfo>
#include <QFile>
#include <QBuffer>
#include <iostream>

const QByteArray MessageFile::fileHeaderByteArray = "MessageFile\n\xCB\xFF\xF0\x0D";
const quint16 MessageFile::fileVersion = 1;

MessageFile::MessageFile(void)
{
}

bool MessageFile::writeFile(QIODevice* ioDevice, const MessageRecList &messageRecList)
{
    m_errorString.clear();

    const bool wasOpen = ioDevice->isOpen();

    if (wasOpen || ioDevice->open(QIODevice::WriteOnly)) {
        QDataStream dataStream(ioDevice);
        dataStream.setVersion(QDataStream::Qt_4_6);

        // Don't use the << operator for QByteArray. See the note in readFile() below.
        dataStream.writeRawData(fileHeaderByteArray.constData(), fileHeaderByteArray.size());
        dataStream << fileVersion;
        dataStream << messageRecList;

        if (!wasOpen) {
            ioDevice->close(); // Only close this if it was opened by this function.
        }
        return true;
    }
    else {
        m_errorString = ioDevice->errorString();
        return false;
    }
    return true;
}

bool MessageFile::readFile(QIODevice* ioDevice, MessageRecList &messageRecList)
{
    messageRecList.clear();
    m_errorString.clear();

    const bool wasOpen = ioDevice->isOpen();

    if (wasOpen || ioDevice->open(QIODevice::ReadOnly)) {
        QDataStream dataStream(ioDevice);
        dataStream.setVersion(QDataStream::Qt_4_6);

        // Note: we could have used the QDataStream << and >> operators on QByteArray but since the first
        // bytes of the stream will be the size of the array, we might end up attempting to allocate
        // a large amount of memory if the wrong file type was read. Instead, we'll just read the
        // same number of bytes that are in the array we are comparing it to. No size was written.
        const int len  = fileHeaderByteArray.size();
        QByteArray actualFileHeaderByteArray(len, '\0');
        dataStream.readRawData(actualFileHeaderByteArray.data(), len);

        if (actualFileHeaderByteArray != fileHeaderByteArray) {
            // prefixes don't match
            m_errorString = QString( "MessageFile::readFile() failed. MessageFile prefix mismatch error." );
            if (!wasOpen) // Only close this if it was opened by this function.
                ioDevice->close();
            return false;
        }

        quint16 actualFileVersion = 0;
        dataStream >> actualFileVersion;

        if (actualFileVersion > fileVersion) {
            // file is from a future version that we don't know how to load
            m_errorString = QString(
                "MessageFile::readFile() failed.\n"
                "MessageFile compatibility error: actualFileVersion = %1 and fileVersion = %2" )
                    .arg( actualFileVersion ).arg( fileVersion );
            if ( !wasOpen ) // Only close this if it was opened by this function.
                ioDevice->close();
            return false;
        }
        dataStream >> messageRecList;
        /*
        try
        {
            // This may throw an exception if one of the MessageRec objects is corrupt or unsupported.
            // For example, if this file is from a future version of this code.
            dataStream >> MessageRecList;
        }
        catch ( const UserException& except )
        {
            // Uses the overloaded ostream << operator defined in UserException.h
            std::cerr << except << std::endl;

            m_errorString = except.message();
            if ( !wasOpen )
                ioDevice->close();
            return false;
        }
        */

        if ( !wasOpen )
            ioDevice->close();
        return true;
    }
    else
    {
        m_errorString = ioDevice->errorString();
        return false;
    }

    return true;
}

bool MessageFile::writeFile(const QString& filePath, const MessageRecList &messageRecList)
{
    m_errorString.clear();

    // We will write to a temp file path so in the event of an error, we don't clobber a good file.
    QFileInfo tempFileInfo(filePath + "-temp");

    // If there's a temp file, remove it. It shouldn't be here anyway.
    if (tempFileInfo.exists()) {
        QFile::remove(tempFileInfo.absoluteFilePath());
    }

    // Write to the temp file.
    QFile file(tempFileInfo.absoluteFilePath());
    if (!writeFile(&file, messageRecList)) { // If write failed, return.
        return false;
    }

    QFileInfo fileInfo(filePath);

    // If there's an existing file, remove the backup file and rename the existing file to backup.
    if (fileInfo.exists()) {
        QFileInfo backupFileInfo(filePath + "-backup");

        if (backupFileInfo.exists()) {
            QFile::remove(backupFileInfo.absoluteFilePath());
        }

        QFile::rename(fileInfo.absoluteFilePath(), backupFileInfo.absoluteFilePath());
    }

    // Rename the temp file to our final name.
    QFile::rename(tempFileInfo.absoluteFilePath(), fileInfo.absoluteFilePath());

    return true;
}

bool MessageFile::readFile(const QString& filePath, MessageRecList &messageRecList)
{
    messageRecList.clear();

    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists()) {
        m_errorString = QString("%1 doesn't exist").arg(filePath);
        return false;
    }

    QFile file(fileInfo.absoluteFilePath());
    return readFile(&file, messageRecList);
}

bool MessageFile::writeFile(const QByteArray& byteArray, const MessageRecList &messageRecList)
{
    // Note: There is a QDataStream constructor that takes a QByteArray but it basically just does what we're doing below.
    // The following version allows us to leverage the common writeFile() and readFile() functions.

    QBuffer buffer(const_cast<QByteArray*>(&byteArray)); // We won't be modifying this but we need a non-const pointer.
    return writeFile(&buffer, messageRecList);
}

bool MessageFile::readFile(QByteArray& byteArray, MessageRecList &messageRecList)
{
    QBuffer buffer(&byteArray);
    return readFile(&buffer, messageRecList);
}
