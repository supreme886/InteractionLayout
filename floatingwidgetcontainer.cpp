#include "floatingwidgetcontainer.h"

#include <QHBoxLayout>

#include "widgetgroup.h"

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "User32.lib")
#endif
#endif

struct FloatingWidgetContainerPrivate {
  FloatingWidgetContainerPrivate(FloatingWidgetContainer *p) : _this(p) {}

  FloatingWidgetContainer *_this{nullptr};
};

FloatingWidgetContainer::FloatingWidgetContainer(QWidget *wg)
    : d(new FloatingWidgetContainerPrivate(this)) {
  setWindowFlags(Qt::Window);

  QHBoxLayout *main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(0, 0, 0, 0);

  main_layout->addWidget(wg);
}

#ifdef Q_OS_WIN
bool FloatingWidgetContainer::nativeEvent(const QByteArray &eventType,
                                          void *message, long *result) {
  QWidget::nativeEvent(eventType, message, result);
  MSG *msg = static_cast<MSG *>(message);
  switch (msg->message) {
    case WM_MOVING: {
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
