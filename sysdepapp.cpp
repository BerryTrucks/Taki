/* $Id: sysdepapp.cpp */
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

#include "sysdepapp.h"
#include <QDesktopWidget>
#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QFont>
#include <QStyle>
#include <QResizeEvent>

SysDepApp::SysDepApp( int & argc, char ** argv) :
    QApplication(argc, argv),
    devType(Misc),
    styleSheet()
{
    findDeviceType();
    applyStyleSheet();
}

SysDepApp::DeviceType SysDepApp::getDeviceType() const
{
    return devType;
}

void SysDepApp::applyStyleSheet()
{
/*
int doubleClickInt = app.doubleClickInterval();
qDebug() << "default doubleClickInt: " << doubleClickInt;
app.setDoubleClickInterval(doubleClickInt*2);
app.setGlobalStrut(QSize(32,32));
*/
//app.setStyleSheet("QLineEdit { background-color: yellow }");

    QFile styleSheetFile;

    //setGlobalStrut(QSize(16,16));
    //QFont fnt("DejaVuSans",16);
    //setFont(fnt);

    // "app/native/qss/"
    switch (devType) {
    case LSeries:
        styleSheetFile.setFileName(QString("qss/") + QString("lseries.qss"));
        break;
    case Z30:
        styleSheetFile.setFileName(QString("qss/") + QString("z30.qss"));
        break;
    case NSeries:
        styleSheetFile.setFileName(QString("qss/") + QString("nseries.qss"));
        break;
    case PlayBookOrg:
        styleSheetFile.setFileName(QString("qss/") + QString("pborg.qss"));
        break;
    case Passport:
        styleSheetFile.setFileName(QString("qss/") + QString("passport.qss"));
        break;
    case Misc:
        styleSheetFile.setFileName(QString("qss/") + QString("misc.qss"));
        //return;
        break;
    }

    qDebug() << "Device type:" << devType << "Style sheet file:" << styleSheetFile.fileName();

    if ( styleSheetFile.open(QFile::ReadOnly) ) {
        styleSheet = QLatin1String(styleSheetFile.readAll());
        setStyleSheet(styleSheet);
    }
    else {
        qWarning() << "Error reading file:" << styleSheetFile.error();
    }

    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Disabled, QPalette::Text, QColor(80, 80, 80));
    pal.setColor(QPalette::Disabled, QPalette::Light, QColor(0, 0, 0, 0));
    QApplication::setPalette(pal);
}

void SysDepApp::findDeviceType()
{
    devType = Misc;
#if defined(Q_OS_QNX)
    // Find on what QNX device we are running
    int w,h;
    w = QApplication::desktop()->width();
    h = QApplication::desktop()->height();

    qDebug() << "findDeviceType w: " << w << "h: " << h;

    if ( (w == 1280 && h == 768) || (w == 768 && h == 1280) ) {
        devType = LSeries;
    }
    else if ( (w == 1280 && h == 720) || (w == 720 && h == 1280) ) {
        devType = Z30;
    }
    else if (w == 720 && h == 720) {
        devType = NSeries;
    }
    else if ( (w == 1024 && h == 600) || (w == 600 && h == 1024) ) {
        devType = PlayBookOrg;
    }
    else if (w == 1440 && h == 1440) {
        devType = Passport;
    }
#endif
}

QString SysDepApp::getTerminalFont() const
{
    QString font;
    switch (devType) {
    case LSeries:
    case Z30:
    case Passport:
        font = "Andale Mono,12";
        break;
    case NSeries:
        font = "Andale Mono,12";
        break;
    case PlayBookOrg:
        //  "Liberation Mono,10"
        font = "Monospace,9";
        break;
    case Misc:
        font = "Courier New,10";
        break;
    default:
        font = "Monospace,9";
        break;
    }
    return font;
}

SysDepApp::AppOrientation SysDepApp::getOrientation() const
{
    int appW = QApplication::desktop()->width();
    int appH = QApplication::desktop()->height();
    //qDebug() << "SysDepApp qapp data w: " << appW << " h: " << appH;
    return (appW > appH) ? Landscape : Portrait;
}

SysDepApp::KeyboardChangeStatus SysDepApp::getKeyboardChangeStatus(
        const QResizeEvent *re) const
{
    // Assume we are running full screen
    switch (devType) {
    case LSeries:
    case Z30:
        if (re->size().height() == QApplication::desktop()->height()) {
            return KbOff;
        }
        // Widget smaller then screen. Keyboard pushing it
        if (re->size().height() < QApplication::desktop()->height()) {
            return KbOn;
        }
        // Widget bigger then screen
        if (re->size().height() > QApplication::desktop()->height()) {
            return KbUnknown;
        }
        break;
    default:
        break;
    }
    return KbUnknown;
}

QRect SysDepApp::getActionMenuSize() const
{
    int menuWidth = 350;
    int appW = QApplication::desktop()->width();
    int appH = QApplication::desktop()->height();
    QRect rect = QRect(appW - menuWidth, 0, menuWidth, appH);
    return rect;
}

QRect SysDepApp::getActionMenuSize(const QSize &minSize) const
{
    int appW, appH;

    /*qDebug() << "Global position" << activeWindow()->mapToGlobal( activeWindow()->pos() );
    qDebug() << "Position" << activeWindow()->pos();
    qDebug() << "W" << activeWindow()->width();
    qDebug() << "H" << activeWindow()->height();*/

    int menuWidth = minSize.width();
#if defined(Q_OS_QNX)
    appW = QApplication::desktop()->width();
    appH = QApplication::desktop()->height();
    // Right side of the screen
    QRect rect = QRect(appW - menuWidth, 0, menuWidth, appH);
#else
    QWidget *mainWin = NULL;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if ((mainWin = qobject_cast<QMainWindow *>(widget))) {
            break;
        }
    }
    if (!mainWin) {
        return QRect();
    }
    appW = mainWin->width();
    appH = mainWin->height();
    QPoint wp = mainWin->pos();
    int frameHeight = mainWin->style()->pixelMetric(QStyle::PM_TitleBarHeight);
    // Right side of the window
    QRect rect = QRect(wp.x() + appW - menuWidth, wp.y() + frameHeight, menuWidth, appH);
#endif
    return rect;
}

QString SysDepApp::getStyleSheet() const
{
    return styleSheet;
}

int SysDepApp::mouseReleaseDistanceWithKinetic() const
{
    int jitter = 1;

    switch (devType) {
    case LSeries:
    case Z30:
    case Passport:
        jitter = 10;
        break;
    case NSeries:
        jitter = 10;
        break;
    case PlayBookOrg:
        jitter = 5;
        break;
    case Misc:
        jitter = 1;
        break;
    default:
        jitter = 1;
        break;
    }
    int ret = QApplication::startDragDistance() * jitter;
    return ret;
}

int SysDepApp::startDragDistanceWithKinetic() const
{
    QApplication::startDragDistance();
    int drag = 1;

    switch (devType) {
    case LSeries:
    case Z30:
    case Passport:
        drag = 100;
        break;
    case NSeries:
        drag = 100;
        break;
    case PlayBookOrg:
        drag = 25;
        break;
    case Misc:
        drag = 1;
        break;
    default:
        drag = 1;
        break;
    }
    int ret = QApplication::startDragDistance() * drag;
    return ret;
}

Qt::ToolButtonStyle SysDepApp::getToolButtonStyle() const
{
    Qt::ToolButtonStyle st;
    switch (devType) {
    case LSeries:
    case Z30:
    case Passport:
        st = Qt::ToolButtonTextUnderIcon;
        break;
    case NSeries:
        st = Qt::ToolButtonIconOnly;
        break;
    default:
        st = Qt::ToolButtonTextUnderIcon;
        break;
    }
    return st;
}

QString SysDepApp::getActiveFrameImg() const
{
    QString imgPath("");

    switch (devType) {
    case LSeries:
    case Z30:
        imgPath = "app/native/res/active-frame-334x396.png";
        break;
    case NSeries:
        imgPath = "app/native/res/active-frame-310x211.png";
        break;
    case Passport:
        imgPath = "app/native/res/active-frame-440x486.png";
    default:
        break;
    }
    return imgPath;
}
