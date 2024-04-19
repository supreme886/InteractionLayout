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
  setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint |
                 Qt::WindowCloseButtonHint);

  QHBoxLayout *main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(0, 0, 0, 0);

  main_layout->addWidget(wg);
  wg->show();

  setWindowTitle(wg->windowTitle());
}

QWidget *FloatingWidgetContainer::getTargetWidget() { return d->m_widget; }

bool FloatingWidgetContainer::event(QEvent *event) {
  switch (event->type()) {
    // case QEvent::NonClientAreaMouseMove:
    //   break;
    // case QEvent::NonClientAreaMouseButtonPress:

    //   break;
    case QEvent::NonClientAreaMouseButtonRelease:
      // case QEvent::NonClientAreaMouseButtonDblClick:
      setWidgetIntoLayout();
      break;
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
  switch (msg->message) {
    case WM_MOVING: {
      DragManager::instance()->moveAndHover(this, QCursor::pos());
    } break;

    // case WM_NCLBUTTONDOWN:
    //   break;

    // case WM_NCLBUTTONDBLCLK:

    //   break;

    // case WM_ENTERSIZEMOVE:
    //   break;

    // case WM_EXITSIZEMOVE:
    //   break;
    default:
      break;
  }
  return Supre::nativeEvent(eventType, message, result);
}
#endif

void FloatingWidgetContainer::setWidgetIntoLayout() {
  if (DragManager::instance()->widgetPlug(d->m_widget, QCursor::pos()))
    deleteLater();
}
