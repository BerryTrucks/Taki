/* $Id: mainwindow.cpp */
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
#include "ui_mainwindow.h"
#include "callhistory.h"
#include "accountnewdialog.h"
#include "settingsdialog.h"
#include "genconfig.h"
#include "generalsettings.h"
#include "sysdepapp.h"
#include "calldetails.h"
#include "helpwindow.h"
#include "helpdoc.h"
#include "callsactionmenu.h"
#include "transferactionmenu.h"
#include "dialeractionmenu.h"
#include "contactsactionmenu.h"
#include "historyactionmenu.h"
#include "accountsactionmenu.h"
#include "phone.h"
#include "projectstrings.h"
#include "messagehistory.h"
#include "messageview.h"
#include "accountstatus.h"

#include <unistd.h>
#include <QSettings>
#include <QDebug>
#include <QStatusBar>
#include <QPlainTextDocumentLayout>
//#include <QElapsedTimer>

#if defined(Q_OS_QNX)
#include <bps/bps.h>
#include <bps/soundplayer.h>
#include <bps/netstatus.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <errno.h>
#ifdef BB10_BLD
#include <time.h>
#include <bps/notification.h>
#include <bps/netstatus.h>
#include <bps/screen_input_guard.h>
#include <bps/navigator_invoke.h>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDialog>
#include <bb/platform/NotificationError>
#include <bb/platform/NotificationResult>
#include <bb/system/SystemUiButton>
using namespace bb::platform;
#endif
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    confIsRec(false),
    confIsMute(false),
    confIsOnSpeaker(false),
    selectedCallId(-1),
    settingsGeneral(new GeneralSettings(this)),
    settingsAccounts(new AccountsSettings(this)),
    statusBar(new QStatusBar(this)),
    messageDB(new MessageDB(this)),
    uriToDial(""),
    logDocument(new QTextDocument(this)),
    cursor(new QTextCursor(logDocument)),
#if defined(Q_OS_QNX)
    callsScroller(new QsKineticScroller(this)),
    historyScroller(new QsKineticScroller(this)),
    contactsScroller(new QsKineticScroller(this)),
    accountsScroller(new QsKineticScroller(this)),
#endif
    accountNameHtmlItemDeligate(new HtmlDelegate()),
    accountVMHtmlItemDeligate(new HtmlDelegate()),
#if defined(Q_OS_QNX)
    bpsH(new BpsHandler(this)),
#endif
    appVersion(QString(ProjectStrings::userAgent) + " " + QString(ProjectStrings::userAgentVersion)),
    windowActive(true),
    netAvailable(false),
    netInterfaceType(0),
    lastDialedNumber("")
{
    QPlainTextDocumentLayout *layout = new QPlainTextDocumentLayout(logDocument);
    logDocument->setDocumentLayout(layout);
    logDocument->setMaximumBlockCount(2000);

    ui->setupUi(this);

    logMsg(appVersion);
    ui->lbLogoVersion->setText(appVersion);

    qRegisterMetaType<Cont>("Cont");
    qRegisterMetaType<CallInfo>("CallInfo");
    qRegisterMetaType<Cdr>("Cdr");
    qRegisterMetaType<Account>("Account");
    qRegisterMetaType<MessageRec>("MessageRec");

    connect(this, SIGNAL(dialInvoke()), this, SLOT(onDialInvoke()));
    connect(this, SIGNAL(reloadPhone()), this, SLOT(onReloadPhone()));

    //geometry = saveGeometry();

    ch = new CallHistory(ui->callHistory, settingsGeneral->callHistoryMaxCalls, this);
    updateCallHistoryTab();
    connect(ch, SIGNAL(dialNumber(const QString &, const QString &, bool)),
            this, SLOT(callUri(const QString &, const QString &, bool)));
    connect(ch, SIGNAL(addNewContcat(const QString &,const QString &)),
            this, SLOT(addressBookAddItem(const QString &, const QString &)));
    connect(ch, SIGNAL(callDetails(const Cdr &)),
            this, SLOT(onCallDetails(const Cdr &)));

    AddressBookSettings abs = {
        settingsGeneral->displayContOrder
    };
    ab = new AddressBook(ui->contacts, abs, this);
    connect(ab, SIGNAL(dialContact(const Cont &)), this, SLOT(onCallContact(const Cont &)));
    connect(ab, SIGNAL(copyContact(const Cont &)), this, SLOT(copyContact(const Cont &)));
    connect(ab, SIGNAL(msgContact(const Cont &)), this, SLOT(onMsgView(const Cont &)));
    updateContactsTab();

    statusBar->setSizeGripEnabled(false);
    //ui->callFrame->layout()->addWidget(statusBar);
    ui->mainLayout->addWidget(statusBar, 1, 0);

    statusBar->addPermanentWidget(ui->btActiveAccount);
    if (!settingsAccounts->getDefaultAccount()) {
        ui->btActiveAccount->hide();
    }

    ui->callFrame->hide();
    ui->confFrame->hide();

#if defined(Q_OS_QNX)
    ui->appMenuFrame->hide();
#endif

    //ui->tabWidget->tabBar()->setTabButton(5, QTabBar::LeftSide, ui->tbActionMenu);
    ui->tabWidget->setCornerWidget(ui->tbActionMenu, Qt::TopRightCorner);
    //ui->tbActionMenu->hide();
    ui->tabWidget->setCurrentIndex(settingsGeneral->defaultTab);

#if defined(Q_OS_QNX)
    callsScroller->enableKineticScrollFor(ui->saCalls);
    historyScroller->enableKineticScrollFor(ui->callHistory);
    contactsScroller->enableKineticScrollFor(ui->contacts);
    accountsScroller->enableKineticScrollFor(ui->twAccounts);
#endif

    ui->frNoAccounts->hide();
    ui->twAccounts->setItemDelegateForColumn(AccountsSettings::ColAccName, accountNameHtmlItemDeligate);
    ui->twAccounts->setItemDelegateForColumn(AccountsSettings::ColAccVM, accountVMHtmlItemDeligate);

    //ui->callHistory->grabGesture(Qt::TapGesture);
    //ui->callHistory->grabGesture(Qt::TapAndHoldGesture);
    //ui->callHistory->grabGesture(Qt::PanGesture);
    //ui->callHistory->grabGesture(Qt::PinchGesture);
    //ui->callHistory->grabGesture(Qt::SwipeGesture);

#ifdef BB10_BLD
#else
    ui->btConfSpeaker->hide();
#endif

    convertV1Settings();

    connect(this, SIGNAL(msgNew(MessageRec *)),
            messageDB, SLOT(onMsgNew(MessageRec*)));
    connect(this, SIGNAL(msgStatus(const MessageRec &)),
            messageDB, SLOT(onMsgStatus(const MessageRec &)));
    connect(this, SIGNAL(msgDeleteChat(const Cont &)),
            messageDB, SLOT(onMsgDeleteChat(const Cont &)));
    connect(this, SIGNAL(msgDeleteAll()),
            messageDB, SLOT(onMsgDeleteAll()));
    connect(this, SIGNAL(msgUpdated()),
            messageDB, SLOT(onMsgUpdated()));

    Phone *phone = new Phone();
    phone->moveToThread(&phoneThread);

    connect(&phoneThread, SIGNAL(finished()), phone, SLOT(deleteLater()));
    connect(phone, SIGNAL(logInfo(const QString &)),
            this, SLOT(logMsg(const QString &)));
    connect(phone, SIGNAL(logStatus(const QString &,int)),
            this, SLOT(logStatusMsg(const QString &,int)));
    connect(phone, SIGNAL(callState(const CallInfo &)),
            this, SLOT(onCallState(const CallInfo &)));
    connect(phone, SIGNAL(mediaState(int,int)), this, SLOT(onMediaState(int,int)));
    connect(phone, SIGNAL(regState(const Account &)),
            this, SLOT(onRegState(const Account &)));
    connect(phone, SIGNAL(callRecorded(int, const QString &, bool)),
            this, SLOT(onCallRecorded(int, const QString &, bool)));
    connect(phone, SIGNAL(getTransfereeNumber(int)),
            this, SLOT(onGetTransfereeNumber(int)));
    connect(phone, SIGNAL(msgStatus(const MessageRec &)),
            this, SLOT(onMsgStatus(const MessageRec &)));
    connect(phone, SIGNAL(msgNew(MessageRec *)),
            this, SLOT(onMsgNew(MessageRec *)));
    connect(phone, SIGNAL(zrtpStatusSecureOn(int,const QString &)),
            this, SLOT(onZrtpStatusSecureOn(int,const QString &)));
    connect(phone, SIGNAL(zrtpStatusSecureOff(int)),
            this, SLOT(onZrtpStatusSecureOff(int)));
    connect(phone, SIGNAL(zrtpStatusShowSas(int,const QString &,int)),
            this, SLOT(onZrtpStatusShowSas(int,const QString &,int)));

    connect(this, SIGNAL(reloadPhoneStack(bool)), phone, SLOT(onReloadPhone(bool)));
    connect(this, SIGNAL(sendDTMF(int,const QString &)),
            phone, SLOT(onSendDTMF(int,const QString &)));
    connect(this, SIGNAL(callContact(const Cont &,int)),
            phone, SLOT(onCallContact(const Cont &,int)));
    connect(this, SIGNAL(callBtnClick(const QString &,int,bool)),
            phone, SLOT(onCallBtnClick(const QString &,int,bool)));
    connect(this, SIGNAL(callTransferBtnClick(const QString &,int, const QString &)),
            phone, SLOT(onCallTransferBtnClick(const QString &,int, const QString &)));
    connect(this, SIGNAL(startWavPlayback(const QString &)),
            phone, SLOT(onStartWavPlayback(const QString &)));
    connect(this, SIGNAL(stopWavPlayback()), phone, SLOT(onStopWavPlayback()));
    connect(this, SIGNAL(newLogLevel(int)), phone, SLOT(onSetLogLevel(int)));
    connect(this, SIGNAL(playTestRing(int)), phone, SLOT(onStartTestRing(int)));
    connect(this, SIGNAL(setConfConnect(int,bool)),
            phone, SLOT(onSetConfConnect(int,bool)));
    connect(this, SIGNAL(confButton(const QString &)),
            phone, SLOT(onConfButtonClicked(const QString &)));
    connect(this, SIGNAL(setSelectedCall(int)),
            phone, SLOT(setSelectedCall(int)));
    connect(this, SIGNAL(msgSend(MessageRec *)),
            phone, SLOT(onMsgNew(MessageRec *)));
    phoneThread.start(QThread::HighPriority);

#if defined(Q_OS_QNX)
    QObject::connect(bpsH, SIGNAL(windowActive()), this, SLOT(onWindowActive()));
    QObject::connect(bpsH, SIGNAL(windowInactive()), this, SLOT(onWindowInactive()));
    QObject::connect(bpsH, SIGNAL(swipeDown()), this, SLOT(onSwipeDown()));
    QObject::connect(bpsH, SIGNAL(netStatus(bool, int)), this, SLOT(onNetStatus(bool, int)));
    QObject::connect(bpsH, SIGNAL(callBtnClicked(const QString &, int)),
                     this, SLOT(onCallBtnClick(const QString &, int)));
#else
    onNetStatus(true);
#endif
}

MainWindow::~MainWindow()
{
    phoneThread.quit();
    if (!phoneThread.wait(30000)) {
        qDebug() << "Terminating Phone thread. Didn't respond in time";
        phoneThread.terminate();
        phoneThread.wait();
    }
    delete ui;
    delete accountNameHtmlItemDeligate;
    delete accountVMHtmlItemDeligate;
}

void MainWindow::logMsg(const QString &text)
{
    if (cursor) {
        cursor->movePosition(QTextCursor::End);
        cursor->insertText(text + "\n");
    }
    else {
        qCritical() << "Log object not found";
    }
}

void MainWindow::logStatusMsg(const QString &text, int timeout)
{
    logMsg("Status bar: " + text);
    statusBar->showMessage(text, timeout);
}

void MainWindow::convertV1Settings()
{
    QSettings settings;
    Account *acnt;
    acnt = new Account();

    settings.beginGroup("SIPAccount");
    acnt->displayName = settings.value("displayName", "").toString();
    acnt->userId = settings.value("userId", "").toString();
    acnt->userPassword = settings.value("userPassword", "").toString();
    acnt->domain = settings.value("domain", "").toString();
    QString stun(settings.value("stun", "").toString());
    acnt->sipRegister = settings.value("sipRegister", false).toBool();
    settings.endGroup();

    if (!stun.isEmpty()) {
        settingsGeneral->stunServer = stun;
        settingsGeneral->useStunServer = true;
        settingsGeneral->save();
    }

    if (acnt->userId.isEmpty() && acnt->domain.isEmpty()) {
        delete acnt;
        return;
    }

    qDebug() << "adding old account";

    settingsAccounts->addAccount(acnt);

    settings.beginGroup("SIPAccount");
    settings.remove("");
    settings.endGroup();
    settings.sync();
}

void MainWindow::onReloadPhone()
{
    settingsAccounts->reloadAccounts();
    settingsAccounts->initAccountWidget(ui->twAccounts);
    updateDefaultAccount();

    if (!settingsAccounts->configuredAccounts()) {
        logStatusMsg(tr("Welcome to Taki! Please configure at least one SIP account."));
        ui->tabWidget->setCurrentIndex(GeneralSettings::TabAccounts);
        ui->frNoAccounts->show();
        ui->twAccounts->hide();
    }
    else {
        ui->frNoAccounts->hide();
        ui->twAccounts->show();

        bool netStatus = netAvailable;
        switch (settingsGeneral->requiredNetworkInterface) {
#if defined(Q_OS_QNX)
        case GeneralSettings::NetWifi:
            if (netInterfaceType != NETSTATUS_INTERFACE_TYPE_WIFI) {
                logMsg("Disable network. Default interface type is not Wi-Fi.");
                netStatus = false;
            }
            break;
        case GeneralSettings::NetCellular:
            if (netInterfaceType != NETSTATUS_INTERFACE_TYPE_CELLULAR) {
                logMsg("Disable network. Default interface type is not cellular.");
                netStatus = false;
            }
            break;
#endif
        default:
            logMsg("Any network interface is acceptable.");
            break;
        }
        emit reloadPhoneStack(netStatus);
    }
}

void MainWindow::dialPadKeyPress()
{
    QObject* callingButton = QObject::sender();
    QString d;

#if defined(Q_OS_QNX)
    soundplayer_prepare_sound("input_keypress");
    soundplayer_play_sound("input_keypress");
#endif

    if (callingButton == ui->pushButton1) {
        d = "1";
    } else if (callingButton == ui->pushButton2) {
        d = "2";
    } else if (callingButton == ui->pushButton3) {
        d = "3";
    } else if (callingButton == ui->pushButton4) {
        d = "4";
    } else if (callingButton == ui->pushButton5) {
        d = "5";
    } else if (callingButton == ui->pushButton6) {
        d = "6";
    } else if (callingButton == ui->pushButton7) {
        d = "7";
    } else if (callingButton == ui->pushButton8) {
        d = "8";
    } else if (callingButton == ui->pushButton9) {
        d = "9";
    } else if (callingButton == ui->pushButton0) {
        d = "0";
    } else if (callingButton == ui->pushButtonStar) {
        d = "*";
    } else if (callingButton == ui->pushButtonPound) {
        d = "#";
    } else if (callingButton == ui->pushButtonCallDp) {
        callDp();
    } else if (callingButton == ui->pushButtonBs) {
        ui->leNumberToDial->backspace();
    }

    if (!d.isEmpty()) {
        ui->leNumberToDial->insert(d);
    }
}

void MainWindow::onSendDTMF(int callId, const QString &key)
{
    emit sendDTMF(callId,key);
}

void MainWindow::copyContact(const Cont &cont)
{
    ui->leNumberToDial->setText(cont.getPhoneNumber());
    ui->tabWidget->setCurrentIndex(GeneralSettings::TabDialer);
}

void MainWindow::callDp()
{
    QString directCallUri(ui->leNumberToDial->text().trimmed());
    if (directCallUri.isEmpty()) {
        if (lastDialedNumber.isEmpty()) {
          logStatusMsg(tr("Please enter the number you wish to call"), 10000);
        }
        else {
            ui->leNumberToDial->setText(lastDialedNumber);
        }
        return;
    }
    else {
       ui->leNumberToDial->clear();
    }
    callUri(directCallUri, "");
}

void MainWindow::callVm(const Account *ac)
{
    if (!ac) {
        logStatusMsg(tr("Acount not found"), 10000);
        return;
    }
    if (ac->vmNumber.isEmpty()) {
        logStatusMsg(tr("Voicemail number is not configured for this account"), 10000);
        voicemainSettings();
        return;
    }
    callUri(ac->vmNumber, ac->vmDTMF);
}

void MainWindow::callUri(const QString &url, const QString &dtmf, bool translateNumber, int transfereeCallId)
{
    logMsg("callUri()");

    Cont cont;
    cont.setPhoneNumber(url);
    cont.setSecStagePhoneNumber(dtmf);
    onCallContact(cont, translateNumber, transfereeCallId);
}

void MainWindow::onCallContact(const Cont &cont, bool translateNumber, int transfereeCallId)
{
    const Account *ac = settingsAccounts->getDefaultAccount();
    if (!ac) {
        logStatusMsg(tr("Acount not found"), 10000);
        return;
    }
    Cont newCont(cont);
    newCont.buildDirectCallUri(*ac, translateNumber);
    newCont.setPjSipAccId(ac->pjSipAccId);

    lastDialedNumber = cont.getPhoneNumber();

    logMsg("onCallContact UI. Uri: " + newCont.getDirectCallUri());

    emit callContact(newCont, transfereeCallId);
}

void MainWindow::onCallBtnClick(const QString &btn, int callId)
{
    logMsg("onCallBtnClick UI: " + btn + " for call: " + QString::number(callId));

    notificationStop(callId);

    if (btn == "btKick") {
        kickConfCall(callId);
        if (getNumberOfCalls(ui->confLayout) <= 1) {
            confButtonClicked("btConfSplit");
        }
    }
    else if (btn == "btCallBack") {
        CallWidget *cw = qobject_cast<CallWidget *>(sender());
        if (cw) {
            QString number(cw->getNumber());
            int transfereeCallId = cw->getTransfereeCallId();
            cw->close();
            callUri(number, "", false, transfereeCallId);
        }
    }
    else if (btn == "btDialpad" || btn == "btDialpadC") {
            actionMenu();
    }
    else {
        logMsg("onCallBtnClick UI emit signal");
        emit callBtnClick(btn, callId, true);
    }
}

void MainWindow::onCallState(const CallInfo &ci)
{
    int call_id = ci.c_id;
    QString state(ci.state_text);
    QString from(ci.remote_info);
    QString last_status(ci.last_status);
    QString last_status_text(ci.last_status_text);
    bool ring = ci.ring;

    if (state == "DISCONNCTD") {
        notificationStop(call_id);

        onCallBtnClick("btKick", call_id);
        if (selectedCallId == call_id) {
            selectedCallId = -1;
        }
    }

    if (state == "CALLING" || state == "INCOMING") {
        QString contactName, contactNumber, domain, contactIcon;

        const Account *acnt = settingsAccounts->getPjSipAccount(ci.acc_id);
        if (!acnt) {
            logMsg("onCallState. Account not found");
            return;
        }
        if (acnt) {
            domain = acnt->domain;
        }

        parseCallerId(from, domain, contactName, contactNumber);

        if (contactName.isEmpty()) {
            Cont cont = ab->findContByNumber(contactNumber);
            contactName = cont.getDisplayName();
            contactIcon = cont.getPhotoFilepath();
        }
        CallWidget *cw = new CallWidget(call_id, ci.transfereeCallId, state,
                                        contactName, contactNumber, contactIcon, this);
        if (!cw) {
            logMsg("onCallState. Unable to create CallWidget");
            return;
        }
        ui->cfLayout->addWidget(cw);
        ui->tabWidget->setCurrentIndex(GeneralSettings::TabCalls);
        ui->phoneLogo->setVisible(false);
        ui->callFrame->setVisible(true);
#ifdef BB10_BLD
        screen_input_guard_request_events(0);
        screen_input_guard_enable();
#endif

        if (state == "INCOMING") {
            if (acnt->autoAnswer) {
                cw->buttonClicked("btAnswer");
            }
            else {
                notificationStart(call_id, contactName, contactNumber);
            }
        }
    }
    else {
        emit setPjCallState(call_id, state, last_status, last_status_text, ring);
    }
}

void MainWindow::onCallEnd(const Cdr &cdr)
{
    ch->addCall(cdr);
    updateCallHistoryTab();
}

void MainWindow::onCallDestroy()
{
    if (!getNumberOfCalls()) {
        ui->tabWidget->setCurrentIndex(settingsGeneral->defaultTab);
        ui->callFrame->setVisible(false);
        ui->phoneLogo->setVisible(true);
#ifdef BB10_BLD
        screen_input_guard_disable();
        screen_input_guard_stop_events(0);
#endif
    }
}

int MainWindow::getNumberOfCalls()
{
    int numRegcalls = getNumberOfCalls(ui->cfLayout);
    int numConfCalls = getNumberOfCalls(ui->confLayout);
    int numCalls = numRegcalls + numConfCalls;
    return numCalls;
}

int MainWindow::getNumberOfCalls(const QLayout *layout)
{
    int numOfCalls = 0;
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *li = layout->itemAt(i);
        CallWidget *cw = qobject_cast<CallWidget *>(li->widget());
        if (cw) {
            ++numOfCalls;
        }
    }
    return numOfCalls;
}

void MainWindow::onMediaState(int call_id, int state)
{
    emit setPjMediaState(call_id, state);
}

void MainWindow::onRegState(const Account &acnt)
{
    Account *ac = settingsAccounts->getAccountById(acnt.accId());
    if (ac) {
        *ac = acnt;
        settingsAccounts->updateAccountWidget(ui->twAccounts, ac);
        updateDefaultAccount();
    }
}

void MainWindow::callHistoryDeleteItem()
{
    if (ch->deleteCurrent()) {
        logStatusMsg(tr("Please select a call"), 5000);
    }
    updateCallHistoryTab();
}

void MainWindow::callHistoryClear()
{
    ch->deleteAll();
    updateCallHistoryTab();
}

void MainWindow::callHistoryDial()
{
   QTreeWidgetItem *item = ui->callHistory->currentItem();
   callHistoryDial(item, 0);
}

void MainWindow::callHistoryDial(QTreeWidgetItem *item, int column)
{
    if (!item) {
        logStatusMsg(tr("Please select a call"), 5000);
        return;
    }
    ch->openCallDetails(item, column);
}

void MainWindow::callHistoryAddContact()
{
    QTreeWidgetItem *item = ui->callHistory->currentItem();
    if (!item) {
        logStatusMsg(tr("Please select a call"), 5000);
        return;
    }
    ch->addToContact(item);
}

void MainWindow::callHistoryToolTip()
{
    //qDebug() << "call history tool tip slot";
}

void MainWindow::callHistoryDetails()
{
    qDebug() << "call history details slot";
    QTreeWidgetItem *item = ui->callHistory->currentItem();
    if (!item) {
        logStatusMsg(tr("Please select a call"), 5000);
        return;
    }
    ch->openCallDetails(item, 1);
}

void MainWindow::onCallDetails(const Cdr &cdr)
{
    CallDetails cd(cdr);
    connect(&cd, SIGNAL(delCallDetail()), ch, SLOT(deleteCurrent()));
    connect(&cd, SIGNAL(addCallDetailToContact()), this, SLOT(callHistoryAddContact()));
    connect(&cd, SIGNAL(startWavPlayback(const QString &)),
            this, SLOT(callHistoryStartPlayback(const QString &)));
    connect(&cd, SIGNAL(stopWavPlayback()),
            this, SLOT(callHistoryStopPlayback()));
#if defined(Q_OS_QNX)
    cd.setWindowState(cd.windowState() ^ Qt::WindowMaximized ^ Qt::WindowActive);
#endif
    cd.exec();
}

void MainWindow::callHistoryStartPlayback()
{
    QTreeWidgetItem *item = ui->callHistory->currentItem();
    if (!item) {
        logStatusMsg(tr("Please select a call"), 5000);
        return;
    }

    QVariant v = item->data(0, Qt::UserRole);
    Cdr cdr = v.value<Cdr>();

    QString fn(cdr.getRecordingPath());
    if (fn.isEmpty()) {
        return;
    }

    emit startWavPlayback(fn);
}

void MainWindow::callHistoryStartPlayback(const QString &fn)
{
    emit startWavPlayback(fn);
}

void MainWindow::callHistoryStopPlayback()
{
    emit stopWavPlayback();
}

void MainWindow::addressBookDeleteItem()
{
    int res = ab->delContact();
    if (res) {
        logStatusMsg(tr("Please select a contact"), 5000);
    }
    updateContactsTab();
}

void MainWindow::addressBookDial()
{
    QTreeWidgetItem *item = ui->contacts->currentItem();
    if (item) {
        addressBookDial(item, 0);
    }
    else {
        logStatusMsg(tr("Please select a contact with the phone number."), 5000);
    }
}

void MainWindow::addressBookDial(QTreeWidgetItem *item, int column)
{
    ab->openContact(item, column);
}

void MainWindow::addressBookMsg()
{
    QTreeWidgetItem *item = ui->contacts->currentItem();
    if (item) {
        addressBookMsg(item, 0);
    }
    else {
        logStatusMsg(tr("Please select a contact with the phone number."), 5000);
    }
}

void MainWindow::addressBookMsg(QTreeWidgetItem *item, int column)
{
    bool forMsg = true;
    ab->openContact(item, column, forMsg);
}

void MainWindow::addressBookAddItem(const QString &name, const QString &number)
{
    int res = ab->addContact(name, number);
    updateContactsTab();
    if (res == QDialog::Accepted) {
        ui->tabWidget->setCurrentIndex(GeneralSettings::TabContacts);
        logStatusMsg(tr("Contact added"), 5000);
    }
}

void MainWindow::addressBookEditItem()
{
    ab->editContact();
}

void MainWindow::addressBookSearch(const QString &searchStr)
{
    ab->load(searchStr);
    updateContactsTab();
}

void MainWindow::onCallSelect(int callId)
{
    selectedCallId = callId;

    emit setSelectedCall(callId);
    emit callActivated(callId);
}

void MainWindow::notificationStart(int call_id,
                                    const QString &displayName,
                                    const QString &displayNumber)
{
#if defined(Q_OS_QNX)
    if (windowActive) {
        return;
    }

    Cdr cdr;
    cdr.setName(displayName);
    cdr.setNumber(displayNumber);

    notification_message_t *nt;

    int ret = notification_message_create(&nt);
    if (ret != BPS_SUCCESS) {
         qWarning() << "Error creating notification event";
         return;
    }

    activeNotifications[call_id] = nt;

    char itemId[20];
    snprintf(itemId, sizeof(itemId), "%d", call_id);
    notification_message_set_item_id(nt, itemId);
    qDebug() << "notification_message_set_item_id Item id:" << itemId;

    notification_message_set_title(nt, "Taki. Incoming call");

    QStringList sl;
    if (!cdr.getName().isEmpty()) {
        sl << cdr.getName();
    }
    if (!cdr.getNumber().isEmpty()) {
        sl << cdr.getDisplayNumber();
    }

    QString display(sl.join("\n"));
    QByteArray baDisplay(display.toLocal8Bit());
    notification_message_set_subtitle(nt, baDisplay.data());

    notification_message_add_prompt_choice(nt, "Answer", "btAnswer");
    notification_message_add_prompt_choice(nt, "Reject", "btReject");
    notification_alert(nt);
#else
    Q_UNUSED(call_id);
    Q_UNUSED(displayName);
    Q_UNUSED(displayNumber);
#endif
}

void MainWindow::notificationStop(int call_id)
{
#if defined(Q_OS_QNX)
    notification_message_t *nt = activeNotifications[call_id];
    if (nt) {
        notification_cancel(nt);
        notification_delete(nt);
        notification_message_destroy(&nt);
        activeNotifications[call_id] = NULL;
    }
#else
    Q_UNUSED(call_id);
#endif
}

void MainWindow::setLogLevel(int level)
{
    level = (level) ? 4 : 3;
    settingsGeneral->logLevel = level;
    settingsGeneral->save();

    emit newLogLevel(level);
}

void MainWindow::addNewAccount()
{
    Account *acnt = new Account();
    AccountNewDialog accDlg(acnt, this);

#if defined(Q_OS_QNX)
    accDlg.showMaximized();
#else
    accDlg.show();
#endif
    int ret = accDlg.exec();
    if (ret != QDialog::Accepted) {
        delete acnt;
        return;
    }

    if (acnt->domain.isEmpty()) {
        logStatusMsg(tr("Account not saved. Domain name can't be empty"));
        delete acnt;
        return;
    }

    logMsg("Adding new account");

    settingsAccounts->addAccount(acnt);

    logStatusMsg(tr("Settings saved"));
    emit reloadPhone();
}

void MainWindow::advancedSettingsHelper(Account *acnt, int loadTab)
{
    SettingsDialog advSettings(acnt, loadTab, this);
    connect(&advSettings, SIGNAL(testRing(int)),
            this, SLOT(startTestRing(int)), Qt::QueuedConnection);

#if defined(Q_OS_QNX)
    advSettings.showMaximized();
#else
    advSettings.show();
#endif
    int ret = advSettings.exec();
    if (ret != QDialog::Accepted) {
        return;
    }

    logMsg("Saving account advanced settings");

    advSettings.getResult();
    if (! acnt->accId()) {
        settingsAccounts->addAccount(acnt);
    }
    else {
        settingsAccounts->saveAccounts();
    }

    logStatusMsg(tr("Settings saved"));
    emit reloadPhone();
}

void MainWindow::advancedSettings()
{
    Account *acnt;
    acnt = settingsAccounts->getSelectedAccount(ui->twAccounts);

    bool newAcc = false;
    if (!acnt) {
        acnt = new Account();
        newAcc = true;
    }

    advancedSettingsHelper(acnt);

    if (newAcc && acnt->accId() == 0) {
        delete acnt;
    }
    return;
}

void MainWindow::voicemainSettings()
{
    Account *ac = settingsAccounts->getSelectedAccount(ui->twAccounts);
    if (ac) {
        advancedSettingsHelper(ac, SettingsDialog::Voicemail);
    }
}

void MainWindow::startTestRing(int ringToneIndex)
{
    emit playTestRing(ringToneIndex);
}

void MainWindow::generalSettingsHelper(int loadTab)
{
#if defined(Q_OS_QNX)
    if (ui->appMenuFrame->isVisible()) {
        ui->appMenuFrame->hide();
    }
#endif

    GenConfig genConfig(settingsGeneral, loadTab, this);
#if defined(Q_OS_QNX)
    genConfig.showMaximized();
#else
    genConfig.show();
#endif
    int ret = genConfig.exec();
    if (ret != QDialog::Accepted) {
        return;
    }

    logMsg("Saving general settings");

    genConfig.getResult();
    settingsGeneral->save();
    logStatusMsg(tr("Settings saved"));

    ch->setMaxCalls(settingsGeneral->callHistoryMaxCalls);

    AddressBookSettings abs = {
        settingsGeneral->displayContOrder
    };
    ab->setSettings(abs);
    ab->load();
    emit reloadPhone();
}

void MainWindow::generalSettings()
{
    generalSettingsHelper();
}

void MainWindow::callHistorySettings()
{
    generalSettingsHelper(GenConfig::History);
}

void MainWindow::addAccount()
{
    addNewAccount();
    return;
}

void MainWindow::deleteAccount()
{
    Account *acnt = settingsAccounts->getSelectedAccount(ui->twAccounts);
    if (acnt) {
        settingsAccounts->deleteAccount(acnt);
        logStatusMsg(tr("Account removed"));
        emit reloadPhone();
    }
    else {
        logStatusMsg(tr("No account selected"));
    }
}

void MainWindow::onAccountSelected()
{
    settingsAccounts->setSelectedAsDefualt(ui->twAccounts);
    updateDefaultAccount();
}

void MainWindow::onAccountItemPressed(QTreeWidgetItem * item, int column)
{
    int accId;
    Account *ac;

    switch (column) {
    case AccountsSettings::ColAccVM:
        accId = item->text(AccountsSettings::ColAccId).toInt();
        ac = settingsAccounts->getAccountById(accId);
        callVm(ac);
        break;
    case AccountsSettings::ColAccInfo:
    {
        accId = item->text(AccountsSettings::ColAccId).toInt();
        ac = settingsAccounts->getAccountById(accId);
        if (ac) {
            AccountStatus accSt(ac, this);
#if defined(Q_OS_QNX)
            accSt.showMaximized();
#else
            accSt.show();
#endif
            accSt.exec();
        }
        break;
    }
    default:
        break;
    }
}

void MainWindow::updateDefaultAccount()
{
    Account *ac = settingsAccounts->getDefaultAccount();
    QString defAccName("");
    if (ac) {
        defAccName = ac->nameShort();
        ui->btActiveAccount->setText(defAccName);
        if (ui->btActiveAccount->isHidden()) {
            ui->btActiveAccount->show();
        }

        QString iconPath = settingsAccounts->getAccountIcon(*ac);
        if (!iconPath.isEmpty()) {
            QIcon finalIcon;
            if (iconPath.startsWith('D')) {
                iconPath.remove(0, 1);
                QIcon icon(iconPath);
                QPixmap pm = icon.pixmap(icon.availableSizes().at(0), QIcon::Disabled);
                finalIcon = QIcon(pm);
            }
            else {
                finalIcon = QIcon(iconPath);
            }
            ui->btActiveAccount->setIcon(finalIcon);
        }
    }
}

void MainWindow::onDefaultAccount() {
    ui->tabWidget->setCurrentIndex(GeneralSettings::TabAccounts);
}

void MainWindow::onAddToConf(int callId1, int callId2)
{
    int i;
    qDebug() << "onAddToConf" << callId1 << callId2;
    QLayoutItem *li1 = NULL;
    QLayoutItem *li2 = NULL;
    bool call1InCf = false;
    bool call2InCf = false;

    int numCallsInConf = 0;

    for (i = 0; i < ui->cfLayout->count(); ++i) {
        QLayoutItem *li = ui->cfLayout->itemAt(i);
        CallWidget *cw = qobject_cast<CallWidget *>(li->widget());
        if (cw) {
            if (cw->getCallId() == callId1) {
                li1 = li;
                call1InCf = true;
            }
            if (cw->getCallId() == callId2) {
                li2 = li;
                call2InCf = true;
            }
        }
    }
    for (i = 0; i < ui->confLayout->count(); ++i) {
        QLayoutItem *li = ui->confLayout->itemAt(i);
        CallWidget *cw = qobject_cast<CallWidget *>(li->widget());
        if (cw) {
            ++numCallsInConf;
            if (cw->getCallId() == callId1) {
                li1 = li;
            }
            if (cw->getCallId() == callId2) {
                li2 = li;
            }
        }
    }

    if (li1 && li2) {
        if (call1InCf) {
            qDebug() << "start conf 1" << call1InCf << "call" << callId1;
            ui->cfLayout->removeItem(li1);
            ui->confLayout->addWidget(li1->widget());
            emit setConfConnect(callId1, true);
            emit callInConf(callId1, true);
        }
        if (call2InCf) {
            qDebug() << "start conf 2" << call2InCf << "call" << callId2;
            ui->cfLayout->removeItem(li2);
            ui->confLayout->addWidget(li2->widget());
            emit setConfConnect(callId2, true);
            emit callInConf(callId2, true);
        }
        ui->confFrame->setVisible(true);
    }
}

void MainWindow::confButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        QString btnName(button->objectName());
        confButtonClicked(btnName);
    }
}

void MainWindow::confButtonClicked(const QString &btnName)
{
    int callId;
    int prevCallId = -1;

    if (btnName == "btConfRec") {
        confIsRec = !confIsRec;
        ui->btConfRec->setProperty("activeConfButton", confIsRec);
        style()->unpolish(ui->btConfRec);
        style()->polish(ui->btConfRec);
    }
    if (btnName == "btConfSpeaker") {
        confIsOnSpeaker = !confIsOnSpeaker;
        ui->btConfSpeaker->setProperty("activeConfButton", confIsOnSpeaker);
        style()->unpolish(ui->btConfSpeaker);
        style()->polish(ui->btConfSpeaker);
    }
    else if (btnName == "btConfSplit") {
        while ((callId = getConfCall()) != -1) {
            kickConfCall(callId);
            prevCallId = callId;
        }
        if (prevCallId != -1) {
            onCallSelect(prevCallId);
        }
        ui->confFrame->setVisible(false);
    }
    else if (btnName == "btConfMute") {
        confIsMute = !confIsMute;
        ui->btConfMute->setProperty("activeConfButton", confIsMute);
        style()->unpolish(ui->btConfMute);
        style()->polish(ui->btConfMute);
    }
    else if (btnName == "btConfHangup") {
        while ((callId = getConfCall()) != -1) {
            kickConfCall(callId);
            onCallBtnClick("btHangup", callId);
        }
        ui->confFrame->setVisible(false);
    }
    emit confButton(btnName);
}

void MainWindow::kickConfCall(int call_id)
{
    for (int i = 0; i < ui->confLayout->count(); ++i) {
        QLayoutItem *li = ui->confLayout->itemAt(i);
        CallWidget *cw = qobject_cast<CallWidget *>(li->widget());
        if (cw) {
            if (cw->getCallId() == call_id) {
                ui->confLayout->removeItem(li);
                ui->cfLayout->addWidget(li->widget());
                emit setConfConnect(call_id, false);
                emit callInConf(cw->getCallId(), false);
            }
        }
    }
}

int MainWindow::getConfCall()
{
    for (int i = 0; i < ui->confLayout->count(); ++i) {
        QLayoutItem *li = ui->confLayout->itemAt(i);
        CallWidget *cw = qobject_cast<CallWidget *>(li->widget());
        if (cw) {
            return cw->getCallId();
        }
    }
    return -1;
}

#ifdef BB10_BLD
void MainWindow::onInvoke(const bb::system::InvokeRequest &request)
{
    uriToDial = request.uri().toString();
    ui->leNumberToDial->setText(uriToDial);
    emit dialInvoke();
}
#endif

void MainWindow::onDialInvoke()
{
    callDp();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress
     || event->type() == QEvent::WindowActivate)
    {
#if defined(Q_OS_QNX)
        if (ui->appMenuFrame->isVisible()) {
            ui->appMenuFrame->hide();
        }
#endif
    }
    return QWidget::event(event);
}

void MainWindow::helpWindow()
{
#if defined(Q_OS_QNX)
    if (ui->appMenuFrame->isVisible()) {
        ui->appMenuFrame->hide();
    }
#endif
    HelpWindow helpWindow(logDocument, settingsGeneral->logLevel, this);
#if defined(Q_OS_QNX)
    helpWindow.showMaximized();
#else
    helpWindow.show();
#endif
    helpWindow.exec();
}

void MainWindow::helpDoc()
{
#if defined(Q_OS_QNX)
    if (ui->appMenuFrame->isVisible()) {
        ui->appMenuFrame->hide();
    }
#endif
    HelpDoc helpDoc(this);
#if defined(Q_OS_QNX)
    helpDoc.showMaximized();
#else
    helpDoc.show();
#endif
    helpDoc.exec();
}

void MainWindow::actionMenu()
{
    ActionMenu *actionMenu = NULL;

    int index = ui->tabWidget->currentIndex();
    switch (index) {
    case GeneralSettings::TabCalls:
        actionMenu = new CallsActionMenu(selectedCallId, this);
        break;
    case GeneralSettings::TabDialer:
        actionMenu = new DialerActionMenu(this);
        break;
    case GeneralSettings::TabContacts:
        actionMenu = new ContactsActionMenu(ab->length(), this);
        break;
    case GeneralSettings::TabHistory:
    {
        bool showPlayer = false;

        QTreeWidgetItem *item = ui->callHistory->currentItem();
        if (item) {
            QVariant v = item->data(0, Qt::UserRole);
            Cdr cdr = v.value<Cdr>();

            QString fn(cdr.getRecordingPath());
            if (!fn.isEmpty()) {
                showPlayer = true;
            }
        }
        actionMenu = new HistoryActionMenu(ch->length(), showPlayer, this);
        break;
    }
    case GeneralSettings::TabAccounts:
        actionMenu = new AccountsActionMenu(settingsAccounts->length(), this);
        break;
    default:
        break;
    }

    if (actionMenu) {
        actionMenu->show();
        actionMenu->exec();
        delete actionMenu;
        activateWindow();
    }
}

CallWidget * MainWindow::getCallWidget(int callId)
{
    int i;
    for (i = 0; i < ui->cfLayout->count(); ++i) {
        QLayoutItem *li = ui->cfLayout->itemAt(i);
        CallWidget *cw = qobject_cast<CallWidget *>(li->widget());
        if (cw && (cw->getCallId() == callId)) {
            return cw;
        }
    }
    for (i = 0; i < ui->confLayout->count(); ++i) {
        QLayoutItem *li = ui->confLayout->itemAt(i);
        CallWidget *cw = qobject_cast<CallWidget *>(li->widget());
        if (cw && (cw->getCallId() == callId)) {
            return cw;
        }
    }
    return NULL;
}

void MainWindow::callsActionMenuPress()
{
    if (selectedCallId == -1) {
        return;
    }

    CallWidget *cw = getCallWidget(selectedCallId);
    if (!cw) {
        return;
    }

    QPushButton *callingButton = qobject_cast<QPushButton *>(sender());
    if (!callingButton) {
        return;
    }

    QString strKeyId(callingButton->accessibleName());
    bool isOk;
    int keyId = strKeyId.toInt(&isOk, 16);

    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, keyId, Qt::NoModifier);
    QCoreApplication::postEvent(cw, event);
    return;
}

void MainWindow::transferActionMenuPress(const QString &btnName, int callId,
                                         const QString &extension)
{
    if (btnName == "btAmAttnXfer" && !extension.isEmpty()) {
        callUri(extension, "", true, callId);
    }
    else {
        emit callTransferBtnClick(btnName, callId, extension);
    }
}

void MainWindow::onGetTransfereeNumber(int callId)
{
    TransferActionMenu *transferMenu = new TransferActionMenu(callId, this);
    if (transferMenu) {
        transferMenu->show();
        transferMenu->exec();
        delete transferMenu;
        activateWindow();
    }
}

void MainWindow::dialerAddContact()
{
    QString name("");
    QString number(ui->leNumberToDial->text());

    addressBookAddItem(name, number);
}

void MainWindow::updateCallHistoryTab()
{
    if (ch->length()) {
        ui->lbNoCalls->hide();
        ui->callHistory->show();
    }
    else {
        ui->lbNoCalls->show();
        ui->callHistory->hide();
    }
}

void MainWindow::updateContactsTab()
{
    if (ab->length()) {
        ui->lbNoContacts->hide();
        ui->contacts->show();
        ui->leSearchContacts->show();
        ui->pbSearchContacts->show();
    }
    else {
        if (ui->leSearchContacts->text().isEmpty()) {
            ui->leSearchContacts->hide();
            ui->pbSearchContacts->hide();
        }
        ui->lbNoContacts->show();
        ui->contacts->hide();
    }
}

void MainWindow::parseCallerId(const QString &from, const QString &domain, QString &name, QString &phone)
{
    QRegExp rxAddrWithName("^(\".+\")\\s+<.+:(.+)@(.+)>$");
    QRegExp rxAddrOnly("^<.+:(.+)@(.+)>$");
    QRegExp rxAddr("^(.+)@(.+)$");

    QString user, dom;
    if (rxAddrWithName.indexIn(from) != -1) {
        name = rxAddrWithName.cap(1);
        user = rxAddrWithName.cap(2);
        dom = rxAddrWithName.cap(3);
    }
    else if (rxAddrOnly.indexIn(from) != -1) {
        user = rxAddrOnly.cap(1);
        dom = rxAddrOnly.cap(2);
    }
    else if (rxAddr.indexIn(from) != -1) {
        user = rxAddr.cap(1);
        dom = rxAddr.cap(2);

        user.remove(QChar('<'));
        dom.remove(QChar('>'));
        user.remove(QRegExp("^sip:"));
    }

    if (dom == domain) {
        phone = user;
    }
    else {
        phone = user + "@" + dom;
    }
    phone = user;
}

#if defined(Q_OS_QNX)
void MainWindow::onWindowActive()
{
    windowActive = true;

    WId winId = this->winId();
    int idle_mode = SCREEN_IDLE_MODE_KEEP_AWAKE;
    int status = screen_set_window_property_iv(screen_window_t(winId), SCREEN_PROPERTY_IDLE_MODE, &idle_mode);
    if (status == -1) {
        qWarning() << "Error setting idle keep awake mode errno:" << errno;
    }

    navigator_window_cover_attribute_t * attr;
    navigator_window_cover_attribute_create(&attr);
    navigator_window_cover_attribute_set_live(attr);
    navigator_window_cover_update(attr);
    navigator_window_cover_attribute_destroy(attr);
}

void MainWindow::onWindowInactive()
{
    windowActive = false;

    WId winId = this->winId();
    int idle_mode = SCREEN_IDLE_MODE_NORMAL;
    int status = screen_set_window_property_iv(screen_window_t(winId), SCREEN_PROPERTY_IDLE_MODE, &idle_mode);
    if (status == -1) {
        qWarning() << "Error setting idle normal mode errno:" << errno;
    }

    navigator_window_cover_attribute_t * attr;
    navigator_window_cover_attribute_create(&attr);

    QString imgPath((qobject_cast<SysDepApp *>(qApp))->getActiveFrameImg());
    QByteArray baImgPath(imgPath.toLocal8Bit());
    navigator_window_cover_attribute_set_file(attr, baImgPath.data());

    navigator_window_cover_update(attr);
    navigator_window_cover_attribute_destroy(attr);
}

void MainWindow::onSwipeDown()
{
    ui->appMenuFrame->setVisible(ui->appMenuFrame->isHidden());
}
#endif

void MainWindow::onNetStatus(bool newNetAvailable, int ifType)
{
    QString interfaceType;
    switch(ifType) {
#if defined(Q_OS_QNX)
    case NETSTATUS_INTERFACE_TYPE_UNKNOWN:
        interfaceType = "UNKNOWN";
        break;
    case NETSTATUS_INTERFACE_TYPE_WIRED:
        interfaceType = "WIRED";
        break;
    case NETSTATUS_INTERFACE_TYPE_WIFI:
        interfaceType = "WIFI";
        break;
    case NETSTATUS_INTERFACE_TYPE_BLUETOOTH_DUN:
        interfaceType = "BLUETOOTH DIALUP";
        break;
    case NETSTATUS_INTERFACE_TYPE_USB:
        interfaceType = "USB";
        break;
    case NETSTATUS_INTERFACE_TYPE_VPN:
        interfaceType = "VPN";
        break;
    case NETSTATUS_INTERFACE_TYPE_BB:
        interfaceType = "BB";
        break;
    case NETSTATUS_INTERFACE_TYPE_CELLULAR:
        interfaceType = "CELLULAR";
        break;
    case 8:
        interfaceType = "P2P";
        break;
#endif
    default:
        interfaceType = QString::number(ifType);
    }

    logMsg("Netstatus event. Net available: "
           + QString(newNetAvailable ? "True" : "False") +
           ". Interface type: " + interfaceType + ".");

    if (newNetAvailable) {
        logStatusMsg(tr("Network is available."));
    }
    else {
        logStatusMsg(tr("Network is not available."));
    }

    netAvailable = newNetAvailable;
    netInterfaceType = ifType;
    emit reloadPhone();
}

void MainWindow::onCallRecorded(int call_id, const QString &fn, bool rec)
{
    emit callRecorded(call_id, fn, rec);
}

void MainWindow::onMsgHistory()
{
#if defined(Q_OS_QNX)
    if (ui->appMenuFrame->isVisible()) {
        ui->appMenuFrame->hide();
    }
#endif
    MessageRecList *historyList = new MessageRecList;
    messageDB->loadHistoryList(historyList);

    MessageHistory mh(historyList, this);
    connect(this, SIGNAL(msgNew(MessageRec *)),
            &mh, SLOT(onMsgNew(MessageRec *)));
    connect(this, SIGNAL(msgStatus(const MessageRec &)),
            &mh, SLOT(onMsgStatus(const MessageRec &)));
    connect(this, SIGNAL(msgDeleteHistoryChat(const Cont &)),
            &mh, SLOT(onMsgDeleteHistoryChat(const Cont &)));
    connect(&mh, SIGNAL(msgView(const Cont &)),
            this, SLOT(onMsgView(const Cont &)));
    connect(&mh, SIGNAL(msgDeleteAll()),
            this, SLOT(onMsgDeleteAll()));
#if defined(Q_OS_QNX)
    mh.showMaximized();
#else
    mh.show();
#endif
    mh.exec();
    delete historyList;
}

void MainWindow::onMsgView(const Cont &contact)
{
    MessageRecList *viewList = new MessageRecList;

    if (contact.getAction() != Cont::CopyMessage) {
        messageDB->loadViewList(contact, viewList);
    }

    MessageView *mv = new MessageView(contact, viewList, this);

    connect(this, SIGNAL(msgNew(MessageRec *)),
            mv, SLOT(onMsgNew(MessageRec *)));
    connect(this, SIGNAL(msgStatus(const MessageRec &)),
            mv, SLOT(onMsgStatus(const MessageRec &)));
    connect(this, SIGNAL(msgDeleteAll()),
            mv, SLOT(onMsgDeleteAll()));
    connect(this, SIGNAL(msgDeleteChat(const Cont &)),
            mv, SLOT(onMsgDeleteChat(const Cont &)));
    connect(mv, SIGNAL(msgSend(MessageRec *)),
            this, SLOT(onMsgSend(MessageRec *)));
    connect(mv, SIGNAL(msgDeleteChat(const Cont &)),
            this, SLOT(onMsgDeleteChat(const Cont &)));

#if defined(Q_OS_QNX)
    mv->showMaximized();
#else
    mv->show();
#endif
    mv->exec();
    bool hasUnread = mv->getHasUnreadMsg();
    delete mv;
    delete viewList;

    if (hasUnread) {
        emit msgUpdated();
    }
}

void MainWindow::onMsgSend(MessageRec *msg)
{
    const Account *ac = settingsAccounts->getDefaultAccount();
    if (!ac) {
        logStatusMsg(tr("Acount not found"), 10000);
        return;
    }

    msg->to.buildDirectCallUri(*ac, true);
    msg->pjSipAccId = ac->pjSipAccId;
    msg->id = messageDB->getNextMsgId();
    emit msgSend(msg);
}

void MainWindow::onMsgNew(MessageRec *msg)
{
    if (!msg) {
        return;
    }

    if (msg->id == 0) {
        msg->id = messageDB->getNextMsgId();
    }
    QString phoneNumber(msg->chatContact.getPhoneNumber());
    logMsg("New IM. Phone->UI. Number: " + phoneNumber);
    msg->chatContact = ab->findContByNumber(phoneNumber);
    msg->chatContact.copyPlatformProperties();
    if (msg->chatContact.getPhoneNumber().isEmpty()) {
        msg->chatContact.setPhoneNumber(phoneNumber);
    }

    if (msg->readStatus == MessageRec::UnRead) {
        msg->notifyStart();
    }

    emit msgNew(msg);
}

void MainWindow::onMsgStatus(const MessageRec &msg)
{
    emit msgStatus(msg);
}

void MainWindow::onMsgDeleteChat(const Cont &chatCont)
{
    emit msgDeleteHistoryChat(chatCont);
    emit msgDeleteChat(chatCont);
}

void MainWindow::onMsgDeleteAll()
{
    emit msgDeleteAll();
}

void MainWindow::onZrtpStatusSecureOn(int call_id, const QString &cipher)
{
    emit zrtpStatusSecureOn(call_id, cipher);
}

void MainWindow::onZrtpStatusSecureOff(int call_id)
{
    emit zrtpStatusSecureOff(call_id);
}

void MainWindow::onZrtpStatusShowSas(int call_id, const QString &sas, int verified)
{
    emit zrtpStatusShowSas(call_id, sas, verified);
}
