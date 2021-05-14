/* $Id: helpwindow.h */
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

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>

class QTextDocument;

#if defined(Q_OS_QNX)
#include "QsKineticScroller.h"
//#include "flickcharm.h"
#endif

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit HelpWindow(QTextDocument *document, int logLevel, QWidget *parent = 0);
    ~HelpWindow();
    
private:
#if defined(Q_OS_QNX)
    QsKineticScroller *helpScroller;
    //FlickCharm *helpScroller2;
#endif
    Ui::HelpWindow *ui;
    QString logPath;
signals:
    void setLogLevelChanged(int);

private slots:
    void logMsgChanged(int);
    void emailLog();
};

#endif // HELPWINDOW_H
