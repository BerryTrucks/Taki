/* $Id: extlineedit.cpp */
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

#include "extlineedit.h"
#include "sysdepapp.h"
#include <QContextMenuEvent>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
#include <QTimer>
//#include <QDebug>

#define CONTENT_MENU_TIMEOUT 450

ExtLineEdit::ExtLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_timer(new QTimer(this)),
    m_timerTimeout(false),
    m_autoRepeat(false),
    m_charReplaced(false),
    m_lastKey(0)
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(CONTENT_MENU_TIMEOUT);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
}

void ExtLineEdit::mousePressEvent(QMouseEvent *e)
{
#if defined(Q_OS_QNX)
    m_timer->start();
#endif
    QLineEdit::mousePressEvent(e);
}

void ExtLineEdit::mouseReleaseEvent(QMouseEvent *e)
{
    m_timer->stop();
    QLineEdit::mouseReleaseEvent(e);

    if (m_timerTimeout) {
        showContextMenu();
        m_timerTimeout = false;
    }
}

void ExtLineEdit::onTimerTimeout()
{
    m_timerTimeout = true;
}

void ExtLineEdit::showContextMenu()
{
    QContextMenuEvent menuEvent(QContextMenuEvent::Other, QPoint(0,0));
    QApplication::sendEvent(this, &menuEvent);
}

QMenu* ExtLineEdit::createStandardContextMenu()
{
    QMenu *popup = new QMenu(this);
    popup->setObjectName(QLatin1String("qt_edit_menu"));
    QAction *action = 0;

    if (!isReadOnly()) {
        action = popup->addAction(QLineEdit::tr("Cu&t"));
        action->setEnabled(!isReadOnly() && hasSelectedText()
                           && echoMode() == QLineEdit::Normal);
        connect(action, SIGNAL(triggered()), SLOT(cut()));
    }

    action = popup->addAction(QLineEdit::tr("&Copy"));
    action->setEnabled(hasSelectedText()
                       && echoMode() == QLineEdit::Normal);
    connect(action, SIGNAL(triggered()), SLOT(copy()));

    if (!isReadOnly()) {
        action = popup->addAction(QLineEdit::tr("&Paste"));
        action->setEnabled(!isReadOnly()
                           && !QApplication::clipboard()->text().isEmpty());
        connect(action, SIGNAL(triggered()), SLOT(paste()));
    }

    return popup;
}

void ExtLineEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *popup = createStandardContextMenu();
    popup->exec(e->globalPos());
    delete popup;

    /*
    setFocus();
    activateWindow();
    parentWidget()->raise();
    */
}

void ExtLineEdit::keyPressEvent(QKeyEvent *e)
{
    //qDebug() << "keyPressEvent" << e;

    SysDepApp::DeviceType devType;
    devType = (qobject_cast<SysDepApp *>(qApp))->getDeviceType();
    if (devType == SysDepApp::NSeries ||
        devType == SysDepApp::Passport)
    {
        if (isLetter(e)) {
            // Char replaced and key was not released
            if (m_charReplaced) {
                //qDebug() << "ignore event";
                return;
            }
            // Same key?
            if (e->key() == m_lastKey) {
                m_autoRepeat = true;
            }

            if (m_autoRepeat) {
                QString editText = text();
                int pos = cursorPosition();
                if (pos > 0) {
                    editText.replace(pos-1, 1, editText.at(pos-1).toUpper());
                    setText(editText);
                    setCursorPosition(pos);
                    m_charReplaced = true;
                }
                return;
            }
            m_lastKey = e->key();
        }
    }
    QLineEdit::keyPressEvent(e);
}

void ExtLineEdit::keyReleaseEvent(QKeyEvent *e)
{
    //qDebug() << "keyReleaseEvent" << e;
    SysDepApp::DeviceType devType;
    devType = (qobject_cast<SysDepApp *>(qApp))->getDeviceType();
    if (devType == SysDepApp::NSeries ||
        devType == SysDepApp::Passport)
    {
        m_autoRepeat = false;
        m_charReplaced = false;
        if (e->key() == m_lastKey) {
            m_lastKey = 0;
        }
    }
    QLineEdit::keyReleaseEvent(e);
}

bool ExtLineEdit::isLetter(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_A:
    case Qt::Key_B:
    case Qt::Key_C:
    case Qt::Key_D:
    case Qt::Key_E:
    case Qt::Key_F:
    case Qt::Key_G:
    case Qt::Key_H:
    case Qt::Key_I:
    case Qt::Key_J:
    case Qt::Key_K:
    case Qt::Key_L:
    case Qt::Key_M:
    case Qt::Key_N:
    case Qt::Key_O:
    case Qt::Key_P:
    case Qt::Key_Q:
    case Qt::Key_R:
    case Qt::Key_S:
    case Qt::Key_T:
    case Qt::Key_U:
    case Qt::Key_V:
    case Qt::Key_W:
    case Qt::Key_X:
    case Qt::Key_Y:
    case Qt::Key_Z:
        if (e->text().contains(QRegExp("[a-zA-Z]"))) {
            //qDebug() << "isLetter true";
            return true;
        }
        break;
    default:
        break;
    }
    //qDebug() << "isLetter false";
    return false;
}
