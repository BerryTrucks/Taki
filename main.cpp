/* $Id: main.cpp */
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

#include "mainwindow.h"
#include "sysdepapp.h"
#include "cdr.h"
#include "projectstrings.h"
//#include <libunwind.h>
#include <QtGui/QApplication>

#ifdef BB10_BLD
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
using namespace bb::system;
#endif

//#define QTAPPDEBUG
#ifdef QTAPPDEBUG
const char *QTDEBUGFILE = "tmp/qt-messages.txt";

void myMessageOutput(QtMsgType type, const char *msg)
{
    FILE *fp;
    fp = fopen(QTDEBUGFILE, "a");
    if (!fp) {
        qCritical() << "Can't open log file. Check permissions";
        return;
    }

    switch (type) {
    case QtDebugMsg:
        fprintf(fp, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(fp, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(fp, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(fp, "Fatal: %s\n", msg);
        abort();
    }
    fclose(fp);
#if BACKTRACE
    showBacktrace();
#endif
}

#if BACKTRACE
void showBacktrace(void)
{
    char name[256];
    unw_cursor_t cursor; unw_context_t uc;
    unw_word_t ip, sp, offp;

    unw_getcontext (&uc);
    unw_init_local (&cursor, &uc);

    while (unw_step(&cursor) > 0) {
        char file[256];
        int line = 0;
        name[0] = '\0';
        unw_get_proc_name (&cursor, name, 256, &offp);
        unw_get_reg (&cursor, UNW_REG_IP, &ip);
        unw_get_reg (&cursor, UNW_REG_SP, &sp);
        printf ("%s ip = %lx, sp = %lx\n", name, (long) ip, (long) sp);
    }
}
#endif

#endif

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(taki);

#if defined(Q_OS_QNX) && defined(QTAPPDEBUG)
    FILE *fp;
    fp = fopen(QTDEBUGFILE, "w");
    if (fp) {
        fclose(fp);
        qInstallMsgHandler(myMessageOutput);
    }
    else {
        qCritical() << "Can't create log file. Check permissions";
    }
#endif

    SysDepApp app(argc, argv);
    QCoreApplication::setOrganizationName(ProjectStrings::organizationName);
    QCoreApplication::setOrganizationDomain(ProjectStrings::organizationDomain);
    QCoreApplication::setApplicationName(ProjectStrings::userAgent);
    //QCoreApplication::setApplicationVersion(ProjectStrings::userAgentVersion);

    MainWindow w;
#if defined(Q_OS_QNX)
    w.showMaximized();
#else
    w.show();
#endif

#ifdef BB10_BLD
    InvokeManager invokeManager;
    QObject::connect(&invokeManager,
                     SIGNAL(invoked(const bb::system::InvokeRequest&)),
                     &w, SLOT(onInvoke(const bb::system::InvokeRequest&)));

    switch(invokeManager.startupMode()) {
    case ApplicationStartupMode::LaunchApplication:
        break;
    case ApplicationStartupMode::InvokeApplication:
        break;
    default:
        break;
    }
#endif

    return app.exec();
}
