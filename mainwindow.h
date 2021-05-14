/* $Id: mainwindow.h */
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef BB10_BLD
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#endif

#include "callinfo.h"
#include "callwidget.h"
#include "account.h"
#include "callhistory.h"
#include "addressbook.h"
#include "generalsettings.h"
#include "accountssettings.h"
#include "QsKineticScroller.h"
#include "messagerec.h"
#include "messagedb.h"
#include "htmldelegate.h"

#if defined(Q_OS_QNX)
#include "bpshandler.h"
#endif

#include <QMainWindow>
#include <QPointer>
#include <QThread>
#include <QHash>

class QTextDocument;
class QTextCursor;

#ifdef BB10_BLD
using namespace bb::system;
#include <bb/platform/Notification>
#include <bb/platform/NotificationDialog>
#include <bb/platform/NotificationError>
#include <bb/platform/NotificationResult>
#include <bb/system/SystemUiButton>
using namespace bb::platform;
#endif

#ifdef BB10_BLD
#include <time.h>
#include <bps/notification.h>
#endif

/*
#ifdef BB10_BLD
#include <QtSensors/QProximityFilter>
#include <QtSensors/QProximityReading>
#include <QtSensors/QProximitySensor>
QTM_USE_NAMESPACE
#endif

#ifdef BB10_BLD
class ProximitySensorFilter : public QProximityFilter
{
    bool filter(QProximityReading *reading);
    bool m_close;
public:
    ProximitySensorFilter();
};
#endif
*/

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QThread phoneThread;
public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool confIsRec;
    bool confIsMute;
    bool confIsOnSpeaker;
    int selectedCallId;

    CallHistory *ch;
    AddressBook *ab;
    GeneralSettings *settingsGeneral;
    AccountsSettings *settingsAccounts;
    //QByteArray geometry;
    QStatusBar *statusBar;
    MessageDB *messageDB;

    QString uriToDial;

    QTextDocument *logDocument;
    QTextCursor *cursor;

#if defined(Q_OS_QNX)
    QsKineticScroller *callsScroller;
    QsKineticScroller *historyScroller;
    QsKineticScroller *contactsScroller;
    QsKineticScroller *accountsScroller;
#endif

    HtmlDelegate *accountNameHtmlItemDeligate;
    HtmlDelegate *accountVMHtmlItemDeligate;

    bool isSpeaker;

/*
#ifdef BB10_BLD
    QProximitySensor proximitySensor;
    QPointer<QDialog> blockScreenDialog;
    ProximitySensorFilter proximitySensorFilter;
#endif
*/

/*
#ifdef BB10_BLD
    pjsua_call_id ntCall_id;
    NotificationDialog *ntDialog;
#endif
*/
#ifdef BB10_BLD
    QHash<int,notification_message_t *> activeNotifications;
#endif

#if defined(Q_OS_QNX)
    BpsHandler *bpsH;
#endif
    QString appVersion;
    bool windowActive;
    bool netAvailable;
    int netInterfaceType;

    QString lastDialedNumber;

    //void loadSettings();
    void callDp();
    void callVm(const Account *ac);
    void parseCallerId(const QString &, const QString &domain, QString &name, QString &phone);
    void convertV1Settings();

    CallWidget *getCallWidget(int callId);
    int getNumberOfCalls();
    int getNumberOfCalls(const QLayout *layout);
    void kickConfCall(int callId);
    //void kickConfCallWithStatus(int callId, bool status, bool anyCall);
    int getConfCall();

    void updateCallHistoryTab();
    void updateContactsTab();
    void generalSettingsHelper(int loadTab = 0);
    void advancedSettingsHelper(Account *acnt, int loadTab = 0);

    void notificationStart(int call_id, const QString &displayName,
                            const QString &displayNumber);
    void notificationStop(int call_id);

    void updateDefaultAccount();

protected:
    bool event(QEvent * event);

public slots:
    void logMsg(const QString &text);
    void logStatusMsg(const QString &text, int timeout = 0);

    void addNewAccount();
    void advancedSettings();
    void generalSettings();
    void voicemainSettings();
    void callHistorySettings();
    void helpWindow();
    void helpDoc();
    void dialPadKeyPress();

    void onCallState(const CallInfo &ci);
    void onMediaState(int, int);
    void onRegState(const Account &);

    void onCallBtnClick(const QString &btn, int callId);
    void onSendDTMF(int callId, const QString &key);

    void onCallEnd(const Cdr &cdr);
    void onCallDestroy();
    void onCallSelect(int callId);
    void onAddToConf(int, int);
    void confButtonClicked();
    void confButtonClicked(const QString &btnName);
    void onCallRecorded(int call_id, const QString &fn, bool rec);

    void setLogLevel(int level);

    void callHistoryToolTip();

#ifdef BB10_BLD
    void onInvoke(const bb::system::InvokeRequest &request);
    //void onNtDialogSelect(bb::platform::NotificationResult::Type);
#endif

    //void testSlot();
    void actionMenu();

    void onAccountSelected();
    void onAccountItemPressed(QTreeWidgetItem * item, int column);

    void onZrtpStatusSecureOn(int call_id, const QString &cipher);
    void onZrtpStatusSecureOff(int call_id);
    void onZrtpStatusShowSas(int call_id, const QString &sas, int verified);

signals:
    void setPjCallState(int call_id, const QString &state,
                        const QString &last_status, const QString &last_status_text,
                        bool ring);
    void setPjMediaState(int, int);
    void callActivated(int);
    void reloadPhone();
    void callInConf(int, bool);
    void callRecorded(int, const QString &fn, bool);
    void dialInvoke();

    void reloadPhoneStack(bool netAvailable);
    void callContact(const Cont &cont, int);
    void callBtnClick(const QString &btn, int callId, bool);
    void callTransferBtnClick(const QString &btnName, int callId, const QString &extension);

    void startWavPlayback(const QString &wavFileName);
    void stopWavPlayback();
    void newLogLevel(int);
    void playTestRing(int);
    void setConfConnect(int,bool);
    void confButton(const QString &);
    void setSelectedCall(int callId);
    void sendDTMF(int,const QString &);

    void msgSend(MessageRec *);
    void msgNew(MessageRec *);
    void msgStatus(const MessageRec &);
    void msgDeleteChat(const Cont &);
    void msgDeleteHistoryChat(const Cont &);
    void msgDeleteAll();
    void msgUpdated();

    void zrtpStatusSecureOn(int call_id, const QString &cipher);
    void zrtpStatusSecureOff(int call_id);
    void zrtpStatusShowSas(int call_id, const QString &sas, int verified);

private slots:
    void callHistoryDial();
    void callHistoryDial(QTreeWidgetItem *item, int column);
    void callHistoryAddContact();
    void callHistoryDeleteItem();
    void callHistoryClear();
    void callHistoryDetails();
    void onCallDetails(const Cdr &cdr);
    void callHistoryStartPlayback();
    void callHistoryStartPlayback(const QString &fn);
    void callHistoryStopPlayback();

    void addressBookDial();
    void addressBookDial(QTreeWidgetItem *item, int column);
    void addressBookMsg();
    void addressBookMsg(QTreeWidgetItem *item, int column);
    void addressBookAddItem(const QString &name = "", const QString &number = "");
    void addressBookEditItem();
    void addressBookDeleteItem();
    void addressBookSearch(const QString &searchStr);

    void callUri(const QString &directCallUri, const QString &dtmf = "",
                 bool translateNumber = true, int transfereeCallId = -1);
    void onCallContact(const Cont &cont,
                       bool translateNumber = true, int transfereeCallId = -1);
    void copyContact(const Cont &cont);

    void onReloadPhone();

    void onDefaultAccount();
    void addAccount();
    void deleteAccount();
    void onDialInvoke();
    void callsActionMenuPress();
    void transferActionMenuPress(const QString &btnName, int callId,
                                 const QString &extension);
    void onGetTransfereeNumber(int callId);

    void dialerAddContact();
    void startTestRing(int ringToneIndex);
#ifdef BB10_BLD
    //void onProximityReadingChanged();
#endif
#if defined(Q_OS_QNX)
    void onWindowActive();
    void onWindowInactive();
    void onSwipeDown();
#endif
    void onNetStatus(bool netAvailable, int ifType = 0);

    void onMsgHistory();
    void onMsgView(const Cont &contact);
    void onMsgSend(MessageRec *msg);
    void onMsgNew(MessageRec *msg);
    void onMsgStatus(const MessageRec &msg);
    void onMsgDeleteChat(const Cont &chatCont);
    void onMsgDeleteAll();
};

#endif // MAINWINDOW_H
