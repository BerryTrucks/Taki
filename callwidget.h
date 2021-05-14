/* $Id: callwidget.h */
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

#ifndef CALLWIDGET_H
#define CALLWIDGET_H

#include "cdr.h"
#include "ui_call.h"
#include <QWidget>

class QTimer;

namespace Ui {
    class CallWidget;
}

class CallWidget : public QWidget
{
    Q_OBJECT

public:
    // Invite session state, as in enum pjsip_inv_state
    enum CallState {
        INV_NULL,
        INV_CALLING,
        INV_INCOMING,
        INV_EARLY,
        INV_CONNECTING,
        INV_CONFIRMED,
        INV_DISCONNECTED
    };
    explicit CallWidget(int call_id, int transfereeCallId, const QString &state,
                        const QString &contactName, const QString &contactNumber,
                        const QString &contactIcon,
                        QWidget *parent = 0);
    ~CallWidget();

    QString getNumber() const;
    int getCallId() const { return callId; }
    int getTransfereeCallId() const { return transfereeCallId; }
    void setCallId(int id) { callId = id; }

protected:
    void mouseReleaseEvent (QMouseEvent *event);
    void mousePressEvent (QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    Ui::CallWidget *ui;

    CallState callState;
    QString name;
    QString number;
    QString iconPath;
    int callId;
    int transfereeCallId;
    QTimer *durationTimer;
    Cdr cdr;
    bool isEarlyRing;
    bool isMuted;
    bool isOnHold;
    bool isActive;
    bool isInConf;
    bool isRecorded;
    bool autoRecord;
    bool isOnSpeaker;

    // member variable to store click position
    QPoint m_lastPoint;
    // member variable - flag of click beginning
    bool m_mouseClick;
    bool dtmfSent;

    void applyPropery(QWidget *w, const char *name, const QVariant &value);
    void setHangupBtnState();
    void setBtnState();
    void setCallStatusMsg();
    void setCallStatusMsg(const QString msg, const QString &logLevel);
    void setName(const QString &n);
    void setNumber(const QString &num);

signals:
    void callBtnClicked(const QString &btnName, int id);
    void sendDTMF(int callId, const QString &key);
    void callEnd(const Cdr &);
    void mouseClickEvent();
    void callSelected(int);
    void addToConf(int, int);
    void redial();

public slots:
    void buttonClicked();
    void buttonClicked(const QString &btn);
    void setPjCallState(int call_id, const QString &state,
                        const QString &last_status,
                        const QString &last_status_text,
                        bool ring);
    void setPjMediaState(int call_id, int);
    void updateDuration();
    void onCallActivate(int);
    void onCallInConf(int call_id, bool inConf);
    void onCallRecorded(int call_id, const QString &fn, bool rec);

    void onZrtpStatusSecureOn(int call_id, const QString &cipher);
    void onZrtpStatusSecureOff(int call_id);
    void onZrtpStatusShowSas(int call_id, const QString &sas, int verified);
};

#endif // CALLWIDGET_H
