#include "floatingwidgetcontainer.h"

#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QWindow>

#include "dragmanager.h"
#include "widgetgroup.h"

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "User32.lib")
#endif
#endif

struct FloatingWidgetContainerPrivate {
  FloatingWidgetContainerPrivate(FloatingWidgetContainer *p, QWidget *w)
      : _this(p), m_widget(w) {}

  FloatingWidgetContainer *_this{nullptr};
  QWidget *m_widget{nullptr};
  bool m_isPressed{0};
  QPoint m_pressPoint;
};

FloatingWidgetContainer::FloatingWidgetContainer(QWidget *wg)
    : d(new FloatingWidgetContainerPrivate(this, wg)) {
  setWindowFlags(Qt::Tool | Qt::WindowMaximizeButtonHint |
                 Qt::WindowCloseButtonHint);

  QHBoxLayout *main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(0, 0, 0, 0);

  main_layout->addWidget(wg);

  setWindowTitle(wg->windowTitle());
}

bool FloatingWidgetContainer::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::NonClientAreaMouseMove:
      // DragManager::instance()->moveAndHover(this, QCursor::pos());
      // return true;
      break;
    case QEvent::NonClientAreaMouseButtonPress:

      break;
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::NonClientAreaMouseButtonDblClick:
      /*QMouseEvent *e = qobject_cast<QMouseEvent *>(event);
      if (e && Qt::LeftButton == e->button())*/
      setWidgetIntoLayout();
      break;
    case QEvent::MouseButtonPress: {
      QMouseEvent *e = qobject_cast<QMouseEvent *>(event);
      if (Qt::LeftButton == e->button()) {
        d->m_isPressed = true;
        d->m_pressPoint = e->pos();
      }
    } break;
    case QEvent::MouseButtonRelease: {
      QMouseEvent *e = qobject_cast<QMouseEvent *>(event);
      if (Qt::LeftButton == e->button()) {
        d->m_isPressed = false;
        d->m_pressPoint = QPoint();
        releaseMouse();
      }
    } break;
    case QEvent::MouseMove: {
      if (d->m_isPressed) {
        QMouseEvent *e = qobject_cast<QMouseEvent *>(event);
        QMargins windowMargins = window()->windowHandle()->frameMargins();
        QPoint windowMarginOffset =
            QPoint(windowMargins.left(), windowMargins.top());
        QPoint pos = e->globalPos() - d->m_pressPoint - windowMarginOffset;
        move(pos);
        DragManager::instance()->moveAndHover(this, QCursor::pos());
      }
    } break;
    default:
      break;
  }
  return Supre::event(event);
}

#ifdef Q_OS_WIN
bool FloatingWidgetContainer::nativeEvent(const QByteArray &eventType,
                                          void *message, long *result) {
  QWidget::nativeEvent(eventType, message, result);
  MSG *msg = static_cast<MSG *>(message);
  qDebug() << Q_FUNC_INFO << msg->message;
  switch (msg->message) {
    case WM_MOVING: {
      DragManager::instance()->moveAndHover(this, QCursor::pos());
    } break;

    case WM_NCLBUTTONDOWN:
      break;

    case WM_NCLBUTTONDBLCLK:

      break;

    case WM_ENTERSIZEMOVE:
      break;

    case WM_EXITSIZEMOVE:
      break;
  }
  return false;
}
#endif

void FloatingWidgetContainer::setWidgetIntoLayout() {
  if (DragManager::instance()->widgetPlug(d->m_widget, QCursor::pos()))
    deleteLater();
}
