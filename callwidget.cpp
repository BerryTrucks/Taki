/* $Id: callwidget.cpp */
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

#include "callwidget.h"
#include "sysdepapp.h"
#include <pjsua.h>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>

CallWidget::CallWidget(int call_id, int transfereeCallId, const QString &state,
                       const QString &contactName, const QString &contactNumber,
                       const QString &contactIcon,
                       QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CallWidget),
    callState(INV_NULL),
    name(contactName),
    number(contactNumber),
    iconPath(contactIcon),
    callId(call_id),
    transfereeCallId(transfereeCallId),
    durationTimer(new QTimer(this)),
    isEarlyRing(false),
    isMuted(false),
    isOnHold(false),
    isActive(false),
    isInConf(false),
    isRecorded(false),
    autoRecord(false),
    isOnSpeaker(false),
    m_lastPoint(0,0),
    m_mouseClick(false),
    dtmfSent(false)
{
    // Register types used in signal/slots
    qRegisterMetaType<Cdr>("Cdr");
    // CallWidget -> MainWin
    connect(this, SIGNAL(callBtnClicked(const QString &, int)),
            parent, SLOT(onCallBtnClick(const QString &, int)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(sendDTMF(int, const QString &)),
            parent, SLOT(onSendDTMF(int, const QString &)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(destroyed()), parent, SLOT(onCallDestroy()),
            Qt::QueuedConnection);
    connect(this, SIGNAL(callEnd(const Cdr &)),
            parent, SLOT(onCallEnd(const Cdr &)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(callSelected(int)),
            parent, SLOT(onCallSelect(int)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(addToConf(int, int)),
            parent, SLOT(onAddToConf(int, int)),
            Qt::QueuedConnection);
    // MainWin -> CallWidget
    connect(parent, SIGNAL(callActivated(int)),
            this, SLOT(onCallActivate(int)),
            Qt::QueuedConnection);
    connect(parent, SIGNAL(setPjCallState(int, const QString &, const QString &,
                                          const QString &, bool)),
            this, SLOT(setPjCallState(int, const QString &, const QString &,
                                      const QString &, bool)),
            Qt::QueuedConnection);
    connect(parent, SIGNAL(setPjMediaState(int, int)),
            this, SLOT(setPjMediaState(int, int)),
            Qt::QueuedConnection);
    connect(parent, SIGNAL(callInConf(int, bool)),
            this, SLOT(onCallInConf(int, bool)),
            Qt::QueuedConnection);
    connect(parent, SIGNAL(callRecorded(int, const QString &, bool)),
            this, SLOT(onCallRecorded(int, const QString &, bool)),
            Qt::QueuedConnection);
    connect(parent, SIGNAL(zrtpStatusSecureOn(int,const QString &)),
            this, SLOT(onZrtpStatusSecureOn(int,const QString &)),
            Qt::QueuedConnection);
    connect(parent, SIGNAL(zrtpStatusSecureOff(int)),
            this, SLOT(onZrtpStatusSecureOff(int)),
            Qt::QueuedConnection);
    connect(parent, SIGNAL(zrtpStatusShowSas(int,const QString &,int)),
            this, SLOT(onZrtpStatusShowSas(int,const QString &,int)),
            Qt::QueuedConnection);

    connect(durationTimer, SIGNAL(timeout()), this, SLOT(updateDuration()));

    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    durationTimer->start(1000);

    cdr.setName(contactName);
    cdr.setNumber(contactNumber);

    setName(contactName);
    setNumber(contactNumber);

#ifdef BB10_BLD
    if (iconPath.isEmpty()) {
        ui->lbPhoto->setPixmap(QPixmap(":/icons/contact.png"));
    }
    else {
        ui->lbPhoto->setPixmap(QPixmap(iconPath));
    }
#else
    ui->lbPhoto->hide();
#endif

    ui->lbDTMF->setText("");
    ui->lbDTMF->hide();

    ui->frZrtp->hide();
    ui->lbZrtpInfo->setText("");
    ui->lbZrtpSAS->setText("");
    ui->lbZrtpCompare->setText("");
    ui->lbZrtpVerify->setText("");
    ui->btZrtpConfirm->hide();
    ui->btZrtpReset->hide();

    ui->callFrame->setProperty("selectedCall", false);
    ui->lbStatus->setProperty("labelStatus", "info");

    // Set foucus policy to be able to receive keyboard events
    setFocusPolicy(Qt::StrongFocus);

    setPjCallState(callId, state, "", "", false);
}

CallWidget::~CallWidget()
{
    delete ui;
}

void CallWidget::updateDuration()
{
    ui->lbTime->setText(cdr.durationStr());
}

// Called by main window
void CallWidget::setPjCallState(int call_id, const QString &state,
                                const QString &last_status,
                                const QString &last_status_text,
                                bool ring)
{
    // Our call?
    if (call_id != callId) {
        return;
    }

    //qDebug() << "setPjCallState" << state;

    if (state == "CALLING") {
        callState = INV_CALLING;
        cdr.setDirection("Out");
    }
    else if (state == "INCOMING") {
        callState = INV_INCOMING;
        cdr.setDirection("In");
    }
    else if (state == "EARLY") {
        callState = INV_EARLY;
        isEarlyRing = ring;
    }
    else if (state == "CONNECTING") {
        callState = INV_CONNECTING;
        cdr.answer();
        emit callSelected(callId);
    }
    else if (state == "CONFIRMED") {
        callState = INV_CONFIRMED;
    }
    else if (state == "DISCONNCTD") {
        callState = INV_DISCONNECTED;

        durationTimer->stop();
        cdr.end(last_status, last_status_text);
        // Save to history
        emit callEnd(cdr);

        // Keep call window for outbound failed calls
        if (cdr.getDirection() == "Out"
                && last_status != "200"
                && last_status != "487")
        {
            ui->lbTime->setText(last_status_text);
            applyPropery(ui->lbTime, "labelStatus", "warning");
        }
        else {
            close();
        }
    }

    setCallStatusMsg();
    setBtnState();
}

// Called by main window
void CallWidget::setPjMediaState(int call_id, int state)
{
    // Our call?
    if (call_id != callId) {
        return;
    }

    //qDebug() << "setPjCallState" << state;

    if (state == PJSUA_CALL_MEDIA_ACTIVE) {
        // Reset Mute when return from "on hold" state, as the call is not muted anymore
        if (isOnHold) {
            isMuted = false;
        }
        isOnHold = false;
        setCallStatusMsg();
    }
    else if (state == PJSUA_CALL_MEDIA_LOCAL_HOLD) {
        isOnHold = true;
        setCallStatusMsg(tr("On hold"), "notice");

        if (isOnSpeaker) {
            isOnSpeaker = false;
            applyPropery(ui->btSpeaker, "activeButton", isOnSpeaker);
        }
    }

    applyPropery(ui->btHold, "activeButton", isOnHold);
    applyPropery(ui->btMute, "activeButton", isMuted);
}

void CallWidget::setBtnState()
{
    switch (callState) {
    case INV_NULL:
        break;
    case INV_INCOMING:
        // [ANSWER SILENCE REJECT]
        ui->frConfCall->setVisible(false);
        ui->frFailedCall->setVisible(false);
        ui->frInboundCall->setVisible(true);
        ui->frOutboundCall->setVisible(false);
        ui->frRegularCall->setVisible(false);

        ui->btRecord->setVisible(false);
        ui->btSpeaker->setVisible(false);
        ui->btAnswer->setVisible(true);
        ui->btMute->setVisible(false);
        ui->btSilence->setVisible(true);
        ui->btHold->setVisible(false);
        ui->btReject->setVisible(true);
        ui->btHangup->setVisible(false);
        ui->btCallBack->setVisible(false);
        ui->btCancel->setVisible(false);
        break;
    case INV_CALLING:
        // [HANGUP MUTE SPEAKER]
        ui->frConfCall->setVisible(false);
        ui->frFailedCall->setVisible(false);
        ui->frInboundCall->setVisible(false);
        ui->frOutboundCall->setVisible(false);
        ui->frRegularCall->setVisible(true);

        ui->btRecord->setVisible(true);
        ui->btSpeaker->setVisible(true);
        ui->btAnswer->setVisible(false);
        ui->btMute->setVisible(true);
        ui->btSilence->setVisible(false);
        ui->btHold->setVisible(true);
        ui->btReject->setVisible(false);
        ui->btHangup->setVisible(true);
        ui->btCallBack->setVisible(false);
        ui->btCancel->setVisible(false);

        ui->btRecord->setDisabled(true);
        ui->btMute->setDisabled(false);
        ui->btDialpad->setDisabled(true);
        ui->btSpeaker->setDisabled(false);
        ui->btTransfer->setDisabled(true);
        ui->btHold->setDisabled(true);
        ui->btHangup->setDisabled(false);
        break;
    case INV_EARLY:
        break;
    case INV_CONNECTING:
    case INV_CONFIRMED:
        // [RECORD MUTE DIALPAD SPEAKER TRANSFER HOLD HANGUP]
        ui->frConfCall->setVisible(false);
        ui->frFailedCall->setVisible(false);
        ui->frInboundCall->setVisible(false);
        ui->frOutboundCall->setVisible(false);
        ui->frRegularCall->setVisible(true);

        ui->btRecord->setVisible(true);
        ui->btSpeaker->setVisible(true);
        ui->btAnswer->setVisible(false);
        ui->btMute->setVisible(true);
        ui->btSilence->setVisible(false);
        ui->btHold->setVisible(true);
        ui->btReject->setVisible(false);
        ui->btHangup->setVisible(true);

        ui->btRecord->setDisabled(false);
        ui->btMute->setDisabled(false);
        ui->btDialpad->setDisabled(false);
        ui->btSpeaker->setDisabled(false);
        ui->btTransfer->setDisabled(false);
        ui->btHold->setDisabled(false);
        ui->btHangup->setDisabled(false);
        break;
    case INV_DISCONNECTED:
        // ["Call Back" "Cancel"]
        ui->frConfCall->setVisible(false);
        ui->frFailedCall->setVisible(true);
        ui->frInboundCall->setVisible(false);
        ui->frOutboundCall->setVisible(false);
        ui->frRegularCall->setVisible(false);

        ui->btRecord->setVisible(false);
        ui->btSpeaker->setVisible(false);
        ui->btAnswer->setVisible(false);
        ui->btMute->setVisible(false);
        ui->btSilence->setVisible(false);
        ui->btHold->setVisible(false);
        ui->btReject->setVisible(false);
        ui->btHangup->setVisible(false);
        ui->btKick->setVisible(false);
        ui->btCallBack->setVisible(true);
        ui->btCancel->setVisible(true);
    default:
        break;
    }
    // For conference swap Hold with Kick, toggle Record
    if (isInConf) {
        ui->frConfCall->setVisible(true);
        ui->frFailedCall->setVisible(false);
        ui->frInboundCall->setVisible(false);
        ui->frOutboundCall->setVisible(false);
        ui->frRegularCall->setVisible(false);

        // XXX not supported yet
        if (!autoRecord) {
            ui->btRecord->setVisible(true);
        }
        ui->btKick->setVisible(true);
        ui->btHold->setVisible(false);
        ui->btSpeaker->setVisible(false);
    }
    else {
        if (ui->frConfCall->isVisible()) {
            ui->frConfCall->setVisible(false);
            ui->frRegularCall->setVisible(true);
        }

        ui->btKick->setVisible(false);
        if (cdr.isAnswered()) {
            ui->btHold->setVisible(true);
#ifdef BB10_BLD
            ui->btSpeaker->setVisible(true);
#endif
            if (!autoRecord) {
                ui->btRecord->setVisible(true);
            }
        }
    }

    // XXX not supported yet
    if (autoRecord) {
        ui->btRecord->setVisible(true);
    }
}

void CallWidget::setHangupBtnState()
{
    ui->btRecord->setDisabled(true);
    ui->btSpeaker->setDisabled(true);
    ui->btAnswer->setDisabled(true);
    ui->btMute->setDisabled(true);
    ui->btSilence->setDisabled(true);
    ui->btHold->setDisabled(true);
    ui->btReject->setDisabled(true);
    ui->btHangup->setDisabled(true);
    ui->btKick->setDisabled(true);
}

void CallWidget::setCallStatusMsg()
{
    switch (callState) {
    case INV_NULL:
        break;
    case INV_CALLING:
        setCallStatusMsg(tr("Calling"), "info");
        break;
    case INV_INCOMING:
        setCallStatusMsg(tr("Ringing"), "info");
        break;
    case INV_EARLY:
        if (isEarlyRing) {
            setCallStatusMsg(tr("Ringing"), "info");
        }
        else {
            if (cdr.getDirection() == "In") {
                setCallStatusMsg(tr("Ringing"), "info");
            }
            else {
                setCallStatusMsg(tr("Calling"), "info");
            }
        }
        break;
    case INV_CONNECTING:
        setCallStatusMsg(tr("Answering"), "info");
        break;
    case INV_CONFIRMED:
        setCallStatusMsg(tr("Answered"), "info");
        break;
    case INV_DISCONNECTED:
        setCallStatusMsg(tr("Call ended"), "warning");
        break;
    }
}

void  CallWidget::setCallStatusMsg(const QString msg, const QString &logLevel)
{
    ui->lbStatus->setText(msg);
    applyPropery(ui->lbStatus, "labelStatus", logLevel);
}

void  CallWidget::applyPropery(QWidget *w, const char *name, const QVariant &value)
{
    w->setProperty(name, value);
    style()->unpolish(w);
    style()->polish(w);
}

void CallWidget::setName(const QString &n)
{
    name = n;
    ui->lbName->setText(name);
}

void CallWidget::setNumber(const QString &num)
{
    number = num;
    ui->lbNumber->setText(cdr.getDisplayNumber());
}

QString CallWidget::getNumber() const
{
    return number;
}

// Handle clicks on Call buttons Hold, Mute, Answer, Hangup.
// Emit signal to parent
void CallWidget::buttonClicked()
{
    QPushButton *button = (QPushButton *)sender();
    QString btnName(button->objectName());
    buttonClicked(btnName);
}

void CallWidget::buttonClicked(const QString &btn)
{
    QString btnName(btn);
    //qDebug() << "Call button" << btnName;

    if (btnName == "btMute") {
        isMuted = !isMuted;
        if (isMuted) {
            setCallStatusMsg(tr("Muted"), "notice");
        }
        else {
            btnName = "btUnMute";
            if (isOnHold) {
                setCallStatusMsg(tr("On hold"), "notice");
            }
            else {
                setCallStatusMsg();
            }
        }
        applyPropery(ui->btMute, "activeButton", isMuted);
    }
    else if(btnName == "btRecord") {
        isRecorded = !isRecorded;
        if (isRecorded) {
        }
        else {
            btnName = "btPauseRecord";
        }
        setBtnState();
        applyPropery(ui->btRecord, "activeButton", isRecorded);
    }
    else if(btnName == "btHangup") {
        // For conference calls first remove from the conference
        if (isInConf) {
            emit callBtnClicked("btKick", callId);
        }
        setCallStatusMsg(tr("Ending call"), "info");
        setHangupBtnState();
        hide();
    }
    else if (btnName == "btHold") {
        if (isOnHold) {
            btnName = "btUnHold";
        }
    }
    else if (btnName == "btSpeaker") {
        isOnSpeaker = !isOnSpeaker;
        if (isOnSpeaker) {
        }
        else {
            btnName = "btHandset";
        }
        applyPropery(ui->btSpeaker, "activeButton", isOnSpeaker);
    }

/*
    if (btnName == "btHold") {
        if (!isActive) {
            emit callSelected(callId);
            return;
        }

    }
*/
    emit callBtnClicked(btnName, callId);
}

void CallWidget::onCallActivate(int call_id)
{
    if (call_id == callId) {
        isActive = true;
    }
    else {
        isActive = false;
    }
    applyPropery(ui->callFrame, "selectedCall", isActive);

    // Set keyboard focus on this wigdet
    setFocus();

    //qDebug() << "called onCallActivate for call" << callId;

    if (isInConf) {
        return;
    }

    if (call_id == callId) {
        if (isOnHold) {
            // UnHold
            //qDebug() << "Send UnHold hold call" << callId;
            emit callBtnClicked("btUnHold", callId);
        }
        else {
            //qDebug() << "do nothing";
        }
    }
    else {
        if (!isOnHold) {
            // Hold
            //qDebug() << "Auto put on Hold call" << callId;
            emit callBtnClicked("btHold", callId);
        }
        else {
            //qDebug() << "do nothing";
        }
    }

/*
    if (isInConf)
        return;

    if (call_id == callId) {
        // Our call? Make it active
        if (isOnHold) {
            // UnHold
            emit callBtnClicked("btUnHold", callId);
        }

        // Don't touch the call if it's already active
        if (isActive)
            return;

        ui->callFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
        ui->callFrame->setMidLineWidth(2);
        //ui->callFrame->setStyleSheet("QFrame { border: 2px solid #009bd9; background: white; }");
        //ui->callFrame->setLineWidth(3);
        //ui->callFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
        //ui->callFrame->setStyleSheet("QFrame { background-color: rgb(220,220,220); }\n");
        isActive = true;
    }
    else {
        // Not our call. Make sure that it's On Hold.
        if (!isOnHold) {
            // Hold
            qDebug() << "Auto put on hold call" << callId;
            emit callBtnClicked("btHold", callId);
        }
        ui->callFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
        ui->callFrame->setMidLineWidth(0);
        //ui->callFrame->setStyleSheet("QFrame { border: 1px solid gray; background: white; }");

        //ui->callFrame->setLineWidth(0);
        //ui->callFrame->setFrameStyle(QFrame::Panel | QFrame::Plain);
        isActive = false;
    }
    */
}

void CallWidget::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "mouse press in call widget";
/*
    if (!isInConf) {
        m_lastPoint = event->pos();
        m_mouseClick = true;
        emit callSelected(callId);
    }
*/
    m_lastPoint = event->pos();
    //m_mouseClick = true;

    QWidget::mousePressEvent(event);
}

void CallWidget::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug() << "mouse release in call widget";
/*
    qDebug() << "last" << m_lastPoint << "current" << event->pos()
             << "len" << (event->pos() - m_lastPoint).manhattanLength()
             << "dragdist" << QApplication::startDragDistance()
             << "call id" << callId;
*/

    // check if cursor moved slightly since click beginning
    if ((event->pos() - m_lastPoint).manhattanLength()
            < (qobject_cast<SysDepApp *>(qApp))->mouseReleaseDistanceWithKinetic())
    {
        emit callSelected(callId);
    }
    QWidget::mouseReleaseEvent(event);
}

void CallWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isInConf) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    if (!(event->buttons() & Qt::LeftButton)) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if ((event->pos() - m_lastPoint).manhattanLength()
         < (qobject_cast<SysDepApp *>(qApp))->startDragDistanceWithKinetic())
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QString dragData(QString::number(callId));
    QByteArray baDragData(dragData.toLocal8Bit());

    mimeData->setData("text/plain", baDragData);
    drag->setMimeData(mimeData);
    drag->setPixmap(QPixmap(":/icons/conf-cursor.png"));
    drag->setHotSpot(QPoint(drag->pixmap().width()/2,
                            drag->pixmap().height()));

    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
    Q_UNUSED(dropAction);
}

void CallWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
}

void CallWidget::dropEvent(QDropEvent *event)
{
    QString dragData(event->mimeData()->text());
    event->acceptProposedAction();
    int dragCallId = dragData.toInt();

    // Same call?
    if (dragCallId == callId)
        return;

    // Start conference
    //qDebug() << "Conference current" << callId << "with" << dragCallId;
    emit addToConf(callId, dragCallId);
}

void CallWidget::onCallInConf(int call_id, bool inConf)
{
    // Our call?
    if (call_id != callId) {
        return;
    }

    if (inConf) {
        isInConf = true;
        if (isOnHold) {
            emit callBtnClicked("btUnHold", callId);
        }
        else {   // Change Button Hold->Kick if required
            setBtnState();
        }
    }
    else {
        //qDebug() << "Conference ended for" << callId;
        isInConf = false;
        setBtnState();
        if (!isOnHold) {
            //qDebug() << "send on hold";
            emit callBtnClicked("btHold", callId);
        }
    }
}

void CallWidget::onCallRecorded(int call_id, const QString &fn, bool rec)
{
    // Our call?
    if (call_id != callId) {
        return;
    }

    if (rec) {
        cdr.setRecordingPath(fn);

        isRecorded = true;
        applyPropery(ui->btRecord, "activeButton", isRecorded);
    }
}

void CallWidget::keyPressEvent(QKeyEvent *event)
{
    /*
    qDebug() << "key event on call" << event->key();
    qDebug() << "modifiers" << event->modifiers();
    qDebug() << "nativeModifiers" << event->nativeModifiers();
    qDebug() << "nativeScanCode" << event->nativeScanCode();
    qDebug() << "nativeVirtualKey" << event->nativeVirtualKey();
    qDebug() << "text" << event->text();
    */

    QString key(event->text());

    if (key.isEmpty()) {
        switch(event->key()) {
        case Qt::Key_0:
            key = "0";
            break;
        case Qt::Key_1:
            key = "1";
            break;
        case Qt::Key_2:
        case Qt::Key_A:
        case Qt::Key_B:
        case Qt::Key_C:
            key = "2";
            break;
        case Qt::Key_3:
        case Qt::Key_D:
        case Qt::Key_E:
        case Qt::Key_F:
            key = "3";
            break;
        case Qt::Key_4:
        case Qt::Key_G:
        case Qt::Key_H:
        case Qt::Key_I:
            key = "4";
            break;
        case Qt::Key_5:
        case Qt::Key_J:
        case Qt::Key_K:
        case Qt::Key_L:
            key = "5";
            break;
        case Qt::Key_6:
        case Qt::Key_M:
        case Qt::Key_N:
        case Qt::Key_O:
            key = "6";
            break;
        case Qt::Key_7:
        case Qt::Key_P:
        case Qt::Key_Q:
        case Qt::Key_R:
        case Qt::Key_S:
            key = "7";
            break;
        case Qt::Key_8:
        case Qt::Key_T:
        case Qt::Key_U:
        case Qt::Key_V:
            key = "8";
            break;
        case Qt::Key_9:
        case Qt::Key_W:
        case Qt::Key_X:
        case Qt::Key_Y:
        case Qt::Key_Z:
            key = "9";
            break;
        case Qt::Key_NumberSign:
            key = "#";
            break;
        case Qt::Key_Asterisk:
            key = "*";
            break;
        case Qt::Key_Escape:
            buttonClicked("btHangup");
            break;
        }
    }

    if (!key.isEmpty()) {
        QString dtmfStr(ui->lbDTMF->text());
        dtmfStr.append(key);
        ui->lbDTMF->setText(dtmfStr);
        if (ui->lbDTMF->isHidden()) {
            ui->lbDTMF->show();
        }

        /*
        QString digits(ui->lbNumber->text());
        if (!dtmfSent) {
            digits.append("\n");
            dtmfSent = true;
        }
        digits.append(key);
        ui->lbNumber->setText(digits);
        */

        //qDebug() << "Send key" << key;
        emit sendDTMF(callId, key);
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

void CallWidget::onZrtpStatusSecureOn(int call_id, const QString &cipher)
{
    qDebug() << Q_FUNC_INFO << call_id << callId << cipher;
    if (call_id != callId) {
        return;
    }
    ui->frZrtp->show();
    ui->lbZrtpInfo->setText(tr("Secure"));
    applyPropery(ui->lbZrtpInfo, "labelStatus", "info");
}

void CallWidget::onZrtpStatusSecureOff(int call_id)
{
    qDebug() << Q_FUNC_INFO << call_id << callId;
    if (call_id != callId) {
        return;
    }
    ui->frZrtp->show();
    ui->lbZrtpInfo->setText(tr("Insecure"));
    applyPropery(ui->lbZrtpInfo, "labelStatus", "warning");
}

void CallWidget::onZrtpStatusShowSas(int call_id, const QString &sas, int verified)
{
    qDebug() << Q_FUNC_INFO << call_id << callId;
    if (call_id != callId) {
        return;
    }
    ui->frZrtp->show();
    if (verified) {
        ui->lbZrtpVerify->setText(tr("verified"));
        applyPropery(ui->lbZrtpVerify, "labelStatus", "info");
        ui->btZrtpConfirm->hide();
        ui->btZrtpReset->show();
    }
    else {
        ui->lbZrtpVerify->setText(tr("not verified"));
        applyPropery(ui->lbZrtpVerify, "labelStatus", "notice");
        ui->btZrtpConfirm->show();
        ui->btZrtpReset->hide();
    }
    ui->lbZrtpCompare->setText(tr("Compare:"));
    if (!sas.isEmpty()) {
        ui->lbZrtpSAS->setText(sas);
    }
}
