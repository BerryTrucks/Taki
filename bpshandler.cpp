/* $Id: bpshandler.cpp */
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

#include "bpshandler.h"
#include <QAbstractEventDispatcher>
#include <QHash>
#include <QCoreApplication>
#include <QDebug>
#include <QPointer>
#include <time.h>
#include <errno.h>
#include <bps/netstatus.h>
#include <bps/navigator.h>
#include <bps/notification.h>
#include <bps/navigator_invoke.h>
#include <bb/AbstractBpsEventHandler>


#include <bps/bps.h>

using namespace bb;

/*
class NavigatorBpsEventHandler: public AbstractBpsEventHandler
{
public:
    NavigatorBpsEventHandler();
    virtual ~NavigatorBpsEventHandler();

    virtual void event(bps_event_t *event);
};

NavigatorBpsEventHandler::NavigatorBpsEventHandler() {
    //subscribe(navigator_get_domain());
    subscribe(notification_get_domain());
    bps_initialize();
    //navigator_request_events(0);
    //notification_request_events(0);
    notification_request_events(0);
}

NavigatorBpsEventHandler::~NavigatorBpsEventHandler() {
    bps_shutdown();
}

void NavigatorBpsEventHandler::event( bps_event_t *event ) {
    int domain = bps_event_get_domain(event);
    qDebug() << "GOT event in NavigatorBpsEventHandler";
    if (domain == navigator_get_domain()) {
        int code = bps_event_get_code(event);
        switch(code) {
            case NAVIGATOR_WINDOW_STATE:
                // Do something
                break;
        }
    }
}
*/

static int idCounter = 0;
static bool s_eventFilterInstalled = 0;
static QAbstractEventDispatcher::EventFilter s_previousEventFilter = 0;
static QHash< int, QPointer<BpsHandler> > s_idToBpsHandlerMap;

static bool s_eventFilter(void *message)
{
    bps_event_t * const event = static_cast<bps_event_t *>(message);
    if (event) {
        //qDebug() << "in s_eventFilter got event. counter: " << idCounter;
        QPointer<BpsHandler> control = s_idToBpsHandlerMap.value(idCounter);
        if (control)
            control->bpsEventHandler(event);
    }
    if (s_previousEventFilter) {
        return s_previousEventFilter(message);
    }
    else {
        return false;
    }
}

BpsHandler::BpsHandler(QObject *parent) :
    QObject(parent),
    m_id(-1),
    prevNetAvailable(false)
{
    //NavigatorBpsEventHandler *handler;
    //handler = new NavigatorBpsEventHandler();

    if (!s_eventFilterInstalled) {
        s_eventFilterInstalled = true;
        s_previousEventFilter =
                QAbstractEventDispatcher::instance()->setEventFilter(s_eventFilter);
    }
    openConnection();
}

BpsHandler::~BpsHandler()
{
    closeConnection();
}

void BpsHandler::openConnection()
{
    m_id = ++idCounter;
    s_idToBpsHandlerMap.insert(m_id, this);

    int status;
    status = netstatus_request_events(0);
    if (status == BPS_FAILURE) {
        qWarning() << "Error requesting netstatus events";
    }
    status = navigator_request_events(0);
    if (status == BPS_FAILURE) {
        qWarning() << "Error requesting navigator events";
    }
    status = notification_request_events(0);
    if (status == BPS_FAILURE) {
        qWarning() << "Error requesting notification events";
    }
    else {
        //qDebug() << "notification_request_events returned:" << status;
    }
}

void BpsHandler::closeConnection()
{
    s_idToBpsHandlerMap.remove(m_id);

#ifdef BB10_BLD
    int status;
    status = navigator_stop_events(0);
    if (status == BPS_FAILURE) {
        qWarning() << "Error stopping navigator events";
    }
    status = netstatus_stop_events(0);
    if (status == BPS_FAILURE) {
        qWarning() << "Error stopping netstatus events";
    }
#endif
}

void BpsHandler::bpsEventHandler(bps_event_t *event)
{
    const int eventDomain = bps_event_get_domain(event);
    const int id = bps_event_get_code(event);

    if (eventDomain == navigator_get_domain()) {
        switch (id) {
        case NAVIGATOR_SWIPE_DOWN:
            qDebug() << "BPS Swipe down";
            emit swipeDown();
            break;
        case NAVIGATOR_WINDOW_ACTIVE:
            qDebug() << "BPS Window active";
            emit windowActive();
            break;
        case NAVIGATOR_WINDOW_INACTIVE:
            qDebug() << "BPS Window inactive";
            emit  windowInactive();
            break;
        default:
            //qDebug() << "BPS unhandled navigator event" << id;
            break;
        }
    }
    else if (eventDomain == netstatus_get_domain()) {
        if (id == NETSTATUS_INFO) {
            netstatus_interface_type_t ifType = NETSTATUS_INTERFACE_TYPE_UNKNOWN;
            // This function has been deprecated - use netstatus_event_get_info()
            // and netstatus_info_get_availability().
            bool netAvailable = netstatus_event_get_availability(event);
            if (netAvailable) {
                const char *defaultInterface = netstatus_event_get_default_interface(event);
                netstatus_interface_details_t *details;
                int ret = netstatus_get_interface_details(defaultInterface, &details);
                if (ret == BPS_SUCCESS) {
                    ifType = netstatus_interface_get_type(details);
                    switch (ifType) {
                    case NETSTATUS_INTERFACE_TYPE_WIFI:
                        qDebug() << "wifi interface" << defaultInterface;
                        break;
                    case NETSTATUS_INTERFACE_TYPE_CELLULAR:
                        qDebug() << "cellular interface" << defaultInterface;
                        break;
                    default:
                        qDebug() << "another interface" << defaultInterface;
                        break;
                    }
                    netstatus_free_interface_details(&details);
                }
            }

            qDebug() << "BPS netstatus netavailable:" << netAvailable;
            //if (netAvailable != prevNetAvailable)
                emit netStatus(netAvailable, ifType);
            //prevNetAvailable = netAvailable;
        }
    }
    else if (eventDomain == notification_get_domain()) {
        if (id == NOTIFICATION_CHOICE) {
            QString context(notification_event_get_context(event));
            QString item_id(notification_event_get_item_id(event));
            qDebug() << "Notification choice event. Context" << context << "item_id" << item_id;

            // context holds button name e.g. 'btAnswer'
            // item_id holds callId
            if (!context.isEmpty() && !item_id.isEmpty()) {
                emit callBtnClicked(context, item_id.toInt());
                /*
                navigator_invoke_invocation_t *invoke = NULL;
                navigator_invoke_invocation_create(&invoke);
                // set invocation action and type
                navigator_invoke_invocation_set_action(invoke, "bb.action.OPEN");
                navigator_invoke_invocation_set_type(invoke, "application/vnd.blackberry.calllog.id");

                // pass arbitrary data (in this example, set screen color value to yellow)
                //int screen_color = 0xffffff00;
                //navigator_invoke_invocation_set_data(invoke, &screen_color, sizeof(int));

                navigator_invoke_invocation_send(invoke);
                navigator_invoke_invocation_destroy(invoke);
                */
            }
        }
    }
    else {
        //qDebug() << "BPS unhandled event. Domain=" << eventDomain << "Code=" << id;
    }
}
