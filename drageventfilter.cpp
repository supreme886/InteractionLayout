#include "drageventfilter.h"

#include <QEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QWindow>

#include "dragmanager.h"

struct DragEventFilterPrivate {
  DragEventFilterPrivate(DragEventFilter *pub) : _this(pub) {}

  DragEventFilter *_this;
  QWidget *m_targetWidget{nullptr};
  bool m_hasPressed{0};
  QPoint m_pressPoint;
};

DragEventFilter::DragEventFilter(QObject *parent)
    : QObject{parent}, d(new DragEventFilterPrivate(this)) {}

void DragEventFilter::setDragWidget(QWidget *w) {
  d->m_targetWidget = w;
  d->m_targetWidget->installEventFilter(this);
}

bool DragEventFilter::dragWidgetEmpty() { return d->m_targetWidget == nullptr; }

void DragEventFilter::beginDrag(const QPoint &point) {}

bool DragEventFilter::eventFilter(QObject *watched, QEvent *event) {
  if (d->m_targetWidget && d->m_targetWidget == watched) {
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
      default:
        break;
    }
  }
  return QObject::eventFilter(watched, event);
}

void DragEventFilter::startDrag(QEvent *event) {
  auto *e = dynamic_cast<QMouseEvent *>(event);
  if (e && e->button() == Qt::LeftButton) {
    d->m_hasPressed = true;
    d->m_pressPoint = e->pos();
    d->m_targetWidget->grabMouse();
  }
}

void DragEventFilter::DragMoving(QEvent *event) {
  if (d->m_hasPressed) {
    QMouseEvent *e = dynamic_cast<QMouseEvent *>(event);
    QMargins windowMargins =
        d->m_targetWidget->window()->windowHandle()->frameMargins();
    QPoint windowMarginOffset =
        QPoint(windowMargins.left(), windowMargins.top());
    QPoint pos = e->globalPos() - d->m_pressPoint - windowMarginOffset;
    d->m_targetWidget->move(pos);
    DragManager::instance()->moveAndHover(d->m_targetWidget, QCursor::pos());
  }
}

void DragEventFilter::endDrag(QEvent *event) {
  auto *e = dynamic_cast<QMouseEvent *>(event);
  if (e && e->button() == Qt::LeftButton) {
    d->m_hasPressed = false;
    d->m_pressPoint = QPoint();
    d->m_targetWidget->releaseMouse();
    d->m_targetWidget->removeEventFilter(this);
    // DragManager::instance()->widgetPlug(d->m_targetWidget, QCursor::pos());
    d->m_targetWidget = nullptr;
  }
}
