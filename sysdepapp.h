/* $Id: sysdepapp.h */
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

#ifndef SYSDEPAPP_H
#define SYSDEPAPP_H

#include <QApplication>

class QResizeEvent;

class SysDepApp : public QApplication
{
    Q_OBJECT

public:
    enum DeviceType
    {
        PlayBookOrg,
        LSeries,    // Z10
        Z30,        // Z30, Z3
        NSeries,    // Q10, Q5
        Passport,   // Passport
        Misc
    };

    enum AppOrientation
    {
        Landscape,
        Portrait
    };

    enum KeyboardChangeStatus
    {
        KbOn,
        KbOff,
        KbUnknown
    };

    explicit SysDepApp(int & argc, char ** argv);
    SysDepApp::DeviceType getDeviceType() const;
    QString getTerminalFont() const;
    SysDepApp::AppOrientation getOrientation() const;
    SysDepApp::KeyboardChangeStatus getKeyboardChangeStatus(const QResizeEvent *re) const;
    QRect getActionMenuSize() const;
    QRect getActionMenuSize(const QSize &minSize) const;
    QString getStyleSheet() const;
    int mouseReleaseDistanceWithKinetic() const;
    int startDragDistanceWithKinetic() const;
    Qt::ToolButtonStyle getToolButtonStyle() const;
    QString getActiveFrameImg() const;

private:
    DeviceType devType;
    QString styleSheet;

    void applyStyleSheet();
    void findDeviceType();
};

#endif // SYSDEPAPP_H
