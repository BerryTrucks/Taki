// Copyright (c) 2011, Razvan Petru
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "QsKineticScroller.h"
#include <QApplication>
#include <QScrollBar>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QAbstractScrollArea>
#include <QMouseEvent>
#include <QEvent>
#include <QTimer>
#include <QList>
#include <QtDebug>
#include <cstddef> // for NULL

// A number of mouse moves are ignored after a press to differentiate
// it from a press & drag.
static const int gMaxIgnoredMouseMoves = 4;
// The timer measures the drag speed & handles kinetic scrolling. Adjusting
// the timer interval will change the scrolling speed and smoothness.
// DM original value was 30
static const int gTimerInterval = 15;
// The speed measurement is imprecise, limit it so that the scrolling is not
// too fast.
// DM original value was 30.
static const int gMaxDecelerationSpeed = 60;
// influences how fast the scroller decelerates
static const int gFriction = 1;

//#define QS_KINETIC_DEBUG // uncomment to see debug messages
#ifdef QS_KINETIC_DEBUG
#define SCROLL_DEBUG() qDebug()
#else
#define SCROLL_DEBUG() QNoDebug()
#endif

class QsKineticScrollerImpl
{
public:
   QsKineticScrollerImpl()
      : scrollArea(NULL)
      , isPressed(false)
      , isMoving(false)
      , lastMouseYPos(0)
      , lastScrollBarPosition(0)
      , velocity(0)
      , ignoredMouseMoves(0)
      , movingFriction(0)
      , dragDistCoefficient(1) {}

   void stopMotion()
   {
      isMoving = false;
      velocity = 0;
      kineticTimer.stop();
      movingFriction = 0;
   }

   QAbstractScrollArea* scrollArea;
   bool isPressed;
   bool isMoving;
   QPoint lastPressPoint;
   QPointF lastTouchPoint;
   int lastMouseYPos;
   qreal lastTouchYPos;
   int lastScrollBarPosition;
   int velocity;
   int ignoredMouseMoves;
   QTimer kineticTimer;
   int movingFriction;
   int dragDistCoefficient;

   QList<QEvent*> ignoreList;
};

QsKineticScroller::QsKineticScroller(QObject *parent)
   : QObject(parent)
   , d(new QsKineticScrollerImpl)
{
   connect(&d->kineticTimer, SIGNAL(timeout()), SLOT(onKineticTimerElapsed()));
}

// needed by smart pointer
QsKineticScroller::~QsKineticScroller()
{
}

void QsKineticScroller::enableKineticScrollFor(QAbstractScrollArea* scrollArea)
{
    if (!scrollArea) {
        Q_ASSERT_X(0, "kinetic scroller", "missing scroll area");
        return;
    }

    // remove existing association
    if (d->scrollArea) {
        QList<QLineEdit*> edits = d->scrollArea->findChildren<QLineEdit*>();
        Q_FOREACH(QLineEdit *edit, edits)
            edit->removeEventFilter(this);
        QList<QTextEdit*> textEdits = d->scrollArea->findChildren<QTextEdit*>();
        Q_FOREACH(QTextEdit *textEdit, textEdits)
            textEdit->viewport()->removeEventFilter(this);
        QList<QComboBox*> combos = d->scrollArea->findChildren<QComboBox*>();
        Q_FOREACH(QComboBox *combo, combos)
            combo->removeEventFilter(this);
        d->scrollArea->viewport()->removeEventFilter(this);
        d->scrollArea->removeEventFilter(this);
        d->scrollArea = NULL;
    }

    // associate
    scrollArea->installEventFilter(this);
    scrollArea->viewport()->installEventFilter(this);
    d->scrollArea = scrollArea;

    QList<QLineEdit*> edits = scrollArea->findChildren<QLineEdit*>();
    Q_FOREACH(QLineEdit *edit, edits)
        edit->installEventFilter(this);
    QList<QTextEdit*> textEdits = scrollArea->findChildren<QTextEdit*>();
    Q_FOREACH(QTextEdit *textEdit, textEdits)
        textEdit->viewport()->installEventFilter(this);
    QList<QComboBox*> combos = scrollArea->viewport()->findChildren<QComboBox*>();
    Q_FOREACH(QComboBox *combo, combos)
        combo->installEventFilter(this);
}

void QsKineticScroller::setDragDistCoefficient(int coef)
{
    d->dragDistCoefficient = coef;
}

// intercepts mouse events to make the scrolling work
bool QsKineticScroller::eventFilter(QObject* object, QEvent* event)
{
#ifdef BB10_BLD
    #define DRAG_DIST_MULT 10
#else
    #define DRAG_DIST_MULT 1
#endif

    if (qobject_cast<QLineEdit*>(object) != NULL) {
        return false;
    }

   const QEvent::Type eventType = event->type();
   const bool isMouseAction = (QEvent::MouseButtonPress == eventType
           || QEvent::MouseButtonRelease == eventType);
   const bool isTouchAction = (QEvent::TouchBegin == eventType
           || QEvent::TouchEnd == eventType);
   const bool isMouseEvent = isMouseAction || QEvent::MouseMove == eventType;
   const bool isTouchEvent = isTouchAction || QEvent::TouchUpdate == eventType;

   if ((!isTouchEvent && !isMouseEvent) || !d->scrollArea) {
     return false;
   }

   //SCROLL_DEBUG() << "QsKineticScroller mouse or touch event" << event << "eventtype" << eventType;

   // This event is ignored
   if (isMouseAction && d->ignoreList.removeAll(event) > 0) {
      return false;
   }

   QMouseEvent * const mouseEvent = static_cast<QMouseEvent*>(event);
   QTouchEvent * const touchEvent = static_cast<QTouchEvent*>(event);

   switch (eventType) {
   case QEvent::MouseButtonPress:
   case QEvent::TouchBegin:
   {
       if (isMouseEvent) {
           SCROLL_DEBUG() << "[scroll] MouseButtonPress pos" << mouseEvent->globalPos();
           d->lastPressPoint = mouseEvent->globalPos();
       }
       if (isTouchEvent) {
           QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
           foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
               SCROLL_DEBUG() << "TP" << touchPoint.pos() << "isprimary" << touchPoint.isPrimary();
           }
           const QTouchEvent::TouchPoint &p0 = touchPoints.first();
           SCROLL_DEBUG() << "[scroll] TouchBegin pos" << p0.screenPos();
           d->lastTouchPoint = p0.screenPos();
       }

       d->isPressed = true;
       d->lastScrollBarPosition = d->scrollArea->verticalScrollBar()->value();
       if( d->isMoving ) // press while kinetic scrolling, so stop
           d->stopMotion();
       break;
   }
   case QEvent::MouseMove:
   {
       //SCROLL_DEBUG() << "[scroll] MouseMove pos" << mouseEvent->globalPos();
       if (!d->isMoving) {
           // A few move events are ignored as "click jitter", but after that we
           // assume that the user is doing a click & drag
           //            if( d->ignoredMouseMoves < gMaxIgnoredMouseMoves )
           //               ++d->ignoredMouseMoves;

           // DM make a desision based on drag distance instead of "click jitter"
           if ((mouseEvent->globalPos() - d->lastPressPoint).manhattanLength()
                   < QApplication::startDragDistance() * DRAG_DIST_MULT)
           {
               /*
                 SCROLL_DEBUG() << "[scroll] ignoring movement."
                                << "event position" << mouseEvent->globalPos()
                                << "last pos" << d->lastPressPoint
                                << "manh length" << (mouseEvent->globalPos() - d->lastPressPoint).manhattanLength()
                                << "drag dist" <<  QApplication::startDragDistance();
                                */
           }
           else {
               /*
                 SCROLL_DEBUG() << "[scroll] moving"
                               << "event position" << mouseEvent->globalPos()
                               << "last pos" << d->lastPressPoint
                               << "manh length" << (mouseEvent->globalPos() - d->lastPressPoint).manhattanLength()
                               << "drag dist" <<  QApplication::startDragDistance();
                               */
               d->ignoredMouseMoves = 0;
               d->isMoving = true;
               d->lastMouseYPos = mouseEvent->globalPos().y();
               if (!d->kineticTimer.isActive()) {
                   d->kineticTimer.start(gTimerInterval);
               }
           }
       }
       else {
           // manual scroll
           if (d->isPressed) { // DM don't jump if its not a "hold and move" scrolling
               const int dragDistance = mouseEvent->globalPos().y() - d->lastPressPoint.y();
               const int finalScrollPos = d->lastScrollBarPosition - dragDistance/d->dragDistCoefficient;
               d->scrollArea->verticalScrollBar()->setValue(finalScrollPos);
               SCROLL_DEBUG() << "[scroll] manual scroll to" << finalScrollPos;
           }
       }
       break;
   }
   case QEvent::TouchUpdate:
   {
       QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
       const QTouchEvent::TouchPoint &p0 = touchPoints.first();
       //SCROLL_DEBUG() << "[scroll] TouchUpdate pos" << p0.screenPos();

       if (!d->isMoving) {
           // DM make a desision based on drag distance instead of "click jitter"
           if ((p0.screenPos() - d->lastTouchPoint).manhattanLength()
                   < QApplication::startDragDistance() * DRAG_DIST_MULT) {
               SCROLL_DEBUG() << "[scroll] ignoring movement."
                              << "event position" << p0.screenPos()
                              << "last pos" << d->lastTouchPoint
                              << "manh length" << (p0.screenPos() - d->lastTouchPoint).manhattanLength()
                              << "drag dist" <<  QApplication::startDragDistance();
           }
           else {
               SCROLL_DEBUG() << "[scroll] moving"
                              << "event position" << p0.screenPos()
                              << "last pos" << d->lastTouchPoint
                              << "manh length" << (p0.screenPos() - d->lastTouchPoint).manhattanLength()
                              << "drag dist" <<  QApplication::startDragDistance();
               d->ignoredMouseMoves = 0;
               d->isMoving = true;
               d->lastTouchYPos = p0.screenPos().y();
               if (!d->kineticTimer.isActive()) {
                   d->kineticTimer.start(gTimerInterval);
               }
           }
       }
       else {
           // manual scroll
           if (d->isPressed) { // DM don't jump if its not a "hold and move" scrolling
               const int dragDistance = p0.screenPos().y() - d->lastTouchPoint.y();
               const int finalScrollPos = d->lastScrollBarPosition - dragDistance;
               d->scrollArea->verticalScrollBar()->setValue(finalScrollPos);
               SCROLL_DEBUG() << "[scroll] manual scroll to" << finalScrollPos;
           }
       }
       break;
   }
   case QEvent::MouseButtonRelease:
   {
       const QPoint& releasePos = mouseEvent->pos();
       SCROLL_DEBUG() << "[scroll] release pos" << releasePos;
       d->isPressed = false;
       d->ignoredMouseMoves = 0;
       // Looks like the user wanted a single click. Simulate the click,
       // as the events were already consumed
       const bool isEditWidget = (NULL != qobject_cast<QLineEdit*>(object)
               || NULL != qobject_cast<QTextEdit*>(object->parent()));
       const bool isCombobox = NULL != qobject_cast<QComboBox*>(object);
       const bool isInterestingObject = (object == d->scrollArea->viewport() || isEditWidget
                                         || isCombobox);
       if (!d->isMoving && isInterestingObject) {
           QMouseEvent* mousePress = new QMouseEvent(QEvent::MouseButtonPress,
                                                     releasePos, Qt::LeftButton,
                                                     Qt::LeftButton, Qt::NoModifier);
           QMouseEvent* mouseRelease = new QMouseEvent(QEvent::MouseButtonRelease,
                                                       releasePos, Qt::LeftButton,
                                                       Qt::LeftButton, Qt::NoModifier);
           // Ignore these two
           d->ignoreList << mousePress;
           d->ignoreList << mouseRelease;

           QApplication::postEvent(object, mousePress);
           QApplication::postEvent(object, mouseRelease);
           if (isEditWidget) {
               SCROLL_DEBUG() << "[scroll] trying to open keyboard";
               QEvent *openKeyboard = new QEvent(QEvent::RequestSoftwareInputPanel);
               QApplication::postEvent(object, openKeyboard);
           }
       }
       break;
   }
   case QEvent::TouchEnd:
   {
       SCROLL_DEBUG() << "[scroll] TouchEnd";
       d->isPressed = false;
       d->ignoredMouseMoves = 0;
       break;
   }
   default:
      break;
   }

   return true; // filter event
}

void QsKineticScroller::onKineticTimerElapsed()
{
   if (d->isPressed && d->isMoving) {
      // the speed is measured between two timer ticks
      const QPoint pos = QCursor::pos();
      const int cursorYPos = pos.y();
      d->velocity = cursorYPos - d->lastMouseYPos;
      d->lastMouseYPos = cursorYPos;
      SCROLL_DEBUG() << "[scroll] cursor Y pos" << cursorYPos << d->lastMouseYPos;
   }
   else if (!d->isPressed && d->isMoving) {
       int friction = d->movingFriction + gFriction;
       //DM slow down faster
       //d->movingFriction++;

      // use the previously recorded speed and gradually decelerate
      d->velocity = qBound(-gMaxDecelerationSpeed, d->velocity, gMaxDecelerationSpeed);
      if( d->velocity > 0 ) {
         d->velocity -= friction;
      }
      else if( d->velocity < 0 ) {
         d->velocity += friction;
      }
      if (qAbs(d->velocity) < qAbs(friction)) {
         d->stopMotion();
      }

      const int scrollBarYPos = d->scrollArea->verticalScrollBar()->value();
      const int finalScrollPos = scrollBarYPos - d->velocity/d->dragDistCoefficient;
      SCROLL_DEBUG() << "[scroll] kinetic scroll to" << finalScrollPos << "with velocity" << d->velocity;
      d->scrollArea->verticalScrollBar()->setValue(finalScrollPos);
   }
   else {
      d->stopMotion();
   }
}
