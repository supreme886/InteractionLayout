#include "iwidget.h"

#include <QLayout>
#include <QLayoutItem>

#include "dragmanager.h"
#include "global.h"
#include "interactionlayout.h"
#include "widgettab.h"

class IWidgetPrivate : public QWidgetPrivate {
  Q_DECLARE_PUBLIC(IWidget)
  IWidgetPrivate() {}
  ~IWidgetPrivate() { qDebug() << Q_FUNC_INFO; }
  bool mousePressEvent(QMouseEvent *event);
  bool mouseDoubleClickEvent(QMouseEvent *event);
  bool mouseMoveEvent(QMouseEvent *event);
  bool mouseReleaseEvent(QMouseEvent *event);
  void nonClientAreaMouseEvent(QMouseEvent *event);

  void plug(QMouseEvent *event);

  void initState(const QPoint &pos);
  void startDrag();
  void endDrag();

  void setResizerActive(bool active);
  void setWindowState(bool floating, const QRect &rect);

 private:
  struct Movestate {
    bool dragging{false};
    QPoint m_press_point;
    QLayoutItem *widgetItem;
  };

  QWidgetResizeHandler *resizer{nullptr};
  Movestate *state{nullptr};
  int m_margin{5};
  TabInfoStruct *m_tabInfo;
};

bool IWidgetPrivate::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    qDebug() << Q_FUNC_INFO << (state == nullptr) << event->pos();
    if (state) return false;
    initState(event->pos());
    return true;
  }
  return false;
}

bool IWidgetPrivate::mouseDoubleClickEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
  }
  return false;
}

bool IWidgetPrivate::mouseMoveEvent(QMouseEvent *event) {
  Q_Q(IWidget);
  bool ret = false;
  if (!state) return ret;

  if (!state->dragging &&
      (event->pos() - state->m_press_point).manhattanLength() >
          QApplication::startDragDistance()) {
    startDrag();
    q->grabMouse();
    ret = true;
  }

  if (state->dragging) {
    QMargins windowMargins = q->window()->windowHandle()->frameMargins();
    QPoint windowMarginOffset =
        QPoint(windowMargins.left(), windowMargins.top());
    QPoint pos = event->globalPos() - state->m_press_point - windowMarginOffset;

    q->move(pos);

    DragManager::instance()->moveAndHover(q, QCursor::pos());

    ret = true;
  }

  return ret;
}

bool IWidgetPrivate::mouseReleaseEvent(QMouseEvent *event) {
  Q_Q(IWidget);
  if (event->button() == Qt::LeftButton && state) {
    // endDrag();
    q->releaseMouse();
    q->activateWindow();
    endDrag();
    return true;  // filter out the event
  }
  return false;
}

void IWidgetPrivate::nonClientAreaMouseEvent(QMouseEvent *event) {
  Q_Q(IWidget);
  switch (event->type()) {
    case QEvent::NonClientAreaMouseButtonPress:
      if (state) return;
      initState(event->pos());
      break;
    case QEvent::NonClientAreaMouseMove:
      startDrag();
      DragManager::instance()->moveAndHover(q, QCursor::pos());
      break;
    case QEvent::NonClientAreaMouseButtonRelease:
      endDrag();
      break;
    case QEvent::NonClientAreaMouseButtonDblClick:
      // _q_toggleTopLevel();
      break;
    default:
      break;
  }
}

void IWidgetPrivate::plug(QMouseEvent *event) {
  // qDebug() << Q_FUNC_INFO << qApp->widgetAt(event->pos());
}

void IWidgetPrivate::initState(const QPoint &pos) {
  if (state != 0) return;
  state = new IWidgetPrivate::Movestate;
  state->m_press_point = pos;
  state->dragging = false;
  state->widgetItem = 0;
}

void IWidgetPrivate::startDrag() {
  Q_Q(IWidget);
  if (state == 0 || state->dragging) return;

  QRect r = q->geometry();
  r.moveTopLeft(q->mapToGlobal(QPoint(0, 30)));
  setWindowState(true, r);

  state->dragging = true;
  state->widgetItem = new QWidgetItem(q);
}

void IWidgetPrivate::endDrag() {
  Q_Q(IWidget);
  Q_ASSERT(state != 0);
  delete state;
  state = 0;
  DragManager::instance()->widgetPlug(q, QCursor::pos());
}

void IWidgetPrivate::setResizerActive(bool active) {
  Q_Q(IWidget);
  if (active && !resizer) {
    resizer = new QWidgetResizeHandler(q);
    resizer->setMovingEnabled(false);
  }
  if (resizer) resizer->setActive(QWidgetResizeHandler::Resize, active);
}

void IWidgetPrivate::setWindowState(bool floating, const QRect &rect) {
  Q_Q(IWidget);

  q->setWindowFlags(Qt::Tool | Qt::WindowMaximizeButtonHint |
                    Qt::WindowCloseButtonHint);
  q->setGeometry(rect);
  q->show();

  QWidget *pw = qobject_cast<QWidget *>(q->parent());
  if (pw) {
    InteractionLayout *layout = qobject_cast<InteractionLayout *>(pw->layout());
    if (layout) {
      state->widgetItem = layout->unplug(q);
    }
  }
}

IWidget::IWidget(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(*new IWidgetPrivate(), parent, flags) {
  Q_D(IWidget);
  // d->m_tabInfo = new TabInfoStruct(this);
  setMouseTracking(true);
}

TabInfoStruct *IWidget::getWidgetTabShared() {
  Q_D(IWidget);
  return d->m_tabInfo;
}

void IWidget::setFloating(bool floating, const QRect &rect) {
  Q_D(IWidget);
  d->setWindowState(floating, rect);
}

bool IWidget::event(QEvent *event) {
  Q_D(IWidget);
  // qDebug() << Q_FUNC_INFO << event->type();
  switch (event->type()) {
    case QEvent::Hide:
      break;
    case QEvent::Show:

      break;
    case QEvent::ApplicationLayoutDirectionChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::StyleChange:
    case QEvent::ParentChange:
      break;
    case QEvent::ZOrderChange: {
      break;
    }
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
      // update(qobject_cast<QDockWidgetLayout *>(this->layout())->titleArea());
      break;
    case QEvent::ContextMenu:
      // if (d->state) {
      //   event->accept();
      //   return true;
      // }
      break;
      // return true after calling the handler since we don't want
      // them to be passed onto the default handlers
    case QEvent::MouseButtonPress:
      if (d->mousePressEvent(static_cast<QMouseEvent *>(event))) return true;
      break;
    case QEvent::MouseButtonDblClick:
      if (d->mouseDoubleClickEvent(static_cast<QMouseEvent *>(event)))
        return true;
      break;
    case QEvent::MouseMove:
      if (d->mouseMoveEvent(static_cast<QMouseEvent *>(event))) return true;
      break;
    case QEvent::MouseButtonRelease:
      if (d->mouseReleaseEvent(static_cast<QMouseEvent *>(event))) return true;
      break;
    case QEvent::NonClientAreaMouseMove:
    case QEvent::NonClientAreaMouseButtonPress:
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::NonClientAreaMouseButtonDblClick:
      d->nonClientAreaMouseEvent(static_cast<QMouseEvent *>(event));
      return true;
    case QEvent::Enter:
    case QEvent::Move:
      // d->moveEvent(static_cast<QMoveEvent *>(event));
      // DragManager::instance()->moveAndHover(this,
      // this->geometry().topLeft());
      break;
    case QEvent::Resize:
      // if the mainwindow is plugging us, we don't want to update undocked
      // geometry
      // if (isFloating() && layout != 0 && layout->pluggingWidget != this)
      //   d->undockedGeometry = geometry();

      // // Usually the window won't get resized while it's being moved, but it
      // can
      // // happen, for example on Windows when moving to a screen with bigger
      // // scale factor (and Qt::AA_EnableHighDpiScaling is enabled). If that
      // // happens we should update state->pressPos, otherwise it will be
      // outside
      // // the window when the window shrinks.
      // if (d->state && d->state->dragging)
      //   d->recalculatePressPos(static_cast<QResizeEvent *>(event));
      break;
    default:
      break;
  }
  return QWidget::event(event);
}

void IWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.fillRect(rect(), QBrush(Qt::blue));
  QWidget::paintEvent(event);
}
