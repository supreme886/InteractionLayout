#include "drageventfilter.h"

#include <QDebug>
#include <QEvent>
#include <QLayout>
#include <QMouseEvent>
#include <QWidget>
#include <QWindow>

#include "dragmanager.h"

struct DragEventFilterPrivate {
  DragEventFilterPrivate(DragEventFilter *pub) : m_this(pub) {}

  void setDragWidget(QWidget *w);

  DragEventFilter *m_this;
  QWidget *m_target_widget{nullptr};
  bool m_has_pressed{0};
  QPoint m_press_point;
};

void DragEventFilterPrivate::setDragWidget(QWidget *w) {
  m_target_widget = w;
  m_target_widget->installEventFilter(m_this);
}

DragEventFilter::DragEventFilter(QObject *parent)
    : QObject{parent}, d(new DragEventFilterPrivate(this)) {}

void DragEventFilter::setDragWidget(QWidget *w) { d->setDragWidget(w); }

bool DragEventFilter::dragWidgetEmpty() {
  return d->m_target_widget == nullptr;
}

void DragEventFilter::beginDrag(const QPoint &point) {}

bool DragEventFilter::eventFilter(QObject *watched, QEvent *event) {
  if (d->m_target_widget && d->m_target_widget == watched) {
    switch (event->type()) {
      case QEvent::MouseButtonPress: {
        startDrag(event);
        return false;
      } break;
      case QEvent::MouseButtonRelease: {
        endDrag(event);
        return false;
      } break;
      case QEvent::MouseMove: {
        DragMoving(event);
        return false;
      } break;
      case QEvent::Show: {
      } break;
      default:
        break;
    }
  }
  return QObject::eventFilter(watched, event);
}

void DragEventFilter::startDrag(QEvent *event) {
  auto *e = dynamic_cast<QMouseEvent *>(event);
  if (e && e->button() == Qt::LeftButton) {
    d->m_has_pressed = true;
    d->m_press_point = d->m_target_widget->mapFromGlobal(e->globalPos());
    d->m_target_widget->grabMouse();
  }
}

void DragEventFilter::DragMoving(QEvent *event) {
  if (d->m_has_pressed) {
    QMouseEvent *e = dynamic_cast<QMouseEvent *>(event);
    QMargins window_margins =
        d->m_target_widget->window()->windowHandle()->frameMargins();
    QPoint window_margin_offset =
        QPoint(window_margins.left(), window_margins.top());
    QPoint pos = e->globalPos() - d->m_press_point - window_margin_offset;
    d->m_target_widget->move(pos);
    DragManager::instance()->moveAndHover(d->m_target_widget, QCursor::pos());
  }
}

void DragEventFilter::endDrag(QEvent *event) {
  auto *e = dynamic_cast<QMouseEvent *>(event);
  if (e && e->button() == Qt::LeftButton) {
    d->m_has_pressed = false;
    d->m_press_point = QPoint();
    d->m_target_widget->releaseMouse();
    d->m_target_widget->removeEventFilter(this);
    if (DragManager::instance()->widgetPlug(
            d->m_target_widget->layout()->itemAt(0)->widget(),
            QCursor::pos())) {
      d->m_target_widget->deleteLater();
      d->m_target_widget = nullptr;
    }
  }
}
