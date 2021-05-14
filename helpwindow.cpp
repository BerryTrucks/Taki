/* $Id: helpwindow.cpp */
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

#include "projectstrings.h"
#include "helpwindow.h"
#include "ui_helpwindow.h"
#include <QScrollBar>
#include <QTextDocument>

#ifdef BB10_BLD
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/PpsObject>
#endif

HelpWindow::HelpWindow(QTextDocument *document, int logLevel, QWidget *parent) :
    QDialog(parent),
#if defined(Q_OS_QNX)
    helpScroller(new QsKineticScroller),
#endif
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(setLogLevelChanged(int)),
            parent, SLOT(setLogLevel(int)));

    ui->logBrowser->setDocument(document);

    if (logLevel > 3)
        ui->cbLogSipMsg->setChecked(Qt::Checked);

    ui->logBrowser->verticalScrollBar()->setValue(ui->logBrowser->verticalScrollBar()->maximum());

    ui->logBrowser->verticalScrollBar()->setPageStep(1);
    ui->logBrowser->verticalScrollBar()->setSingleStep(1);

#if defined(Q_OS_QNX)
    helpScroller->enableKineticScrollFor(ui->logBrowser);
    helpScroller->setDragDistCoefficient(30);
    //helpScroller2 = new FlickCharm(this);
    //helpScroller2->activateOn(ui->logBrowser);
#endif

#if defined(Q_OS_QNX)
    ui->frDesktopControls->hide();
#else
    ui->frBBControls->hide();
#endif
}

HelpWindow::~HelpWindow()
{
    if (!logPath.isEmpty()) {
        QFile::remove(logPath);
    }

#if defined(Q_OS_QNX)
    delete helpScroller;
    //delete helpScroller2;
#endif
    delete ui;
}

void HelpWindow::logMsgChanged(int logLevel)
{
    emit setLogLevelChanged(logLevel);
}

void HelpWindow::emailLog()
{
#ifdef BB10_BLD
    QString addr(ProjectStrings::supportEmail);
    QString subj("Taki support");
    QString text(ui->logBrowser->document()->toPlainText());

    bb::system::InvokeRequest request;
    request.setTarget("sys.pim.uib.email.hybridcomposer");
    request.setAction("bb.action.COMPOSE");
    request.setMimeType("message/rfc822");

    const QString workDir = QDir::currentPath();
    logPath = QString::fromLatin1(ProjectStrings::logfilePath).arg(workDir);
    QString logpathEncoded = QString(QUrl(logPath).toEncoded());

    {
        QFile file(logPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "Error opening log file. Check 'Shared' permissions";
            return;
        }
        QTextStream out(&file);
        out << text;
    }

    QVariantMap dataMap;
    dataMap["to"] = addr;
    dataMap["subject"] = subj;
    dataMap["body"] = "Log file attached";
    dataMap["attachment"] = (QVariantList() << logpathEncoded);

    QVariantMap emailDataMap;
    emailDataMap["data"] = dataMap;

    bool encodingResult;
    request.setData(bb::PpsObject::encode(emailDataMap, &encodingResult));

    bb::system::InvokeManager invokeManager;
    invokeManager.invoke(request);
#endif
}
