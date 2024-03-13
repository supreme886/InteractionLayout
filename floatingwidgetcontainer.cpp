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

FloatingWidgetContainer::FloatingWidgetContainer(WidgetGroup *wg)
    : d(new FloatingWidgetContainerPrivate(this)) {
  setWindowFlags(Qt::Window);

  QHBoxLayout *main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(0, 0, 0, 0);

  main_layout->addWidget(wg);
}

bool FloatingWidgetContainer::nativeEvent(const QByteArray &eventType,
                                          void *message, long *result) {
  QWidget::nativeEvent(eventType, message, result);
  MSG *msg = static_cast<MSG *>(message);
  switch (msg->message) {
    case WM_MOVING: {
      //   if (d->isState(DraggingFloatingWidget)) {
      //     d->updateDropOverlays(QCursor::pos());
      //   }
    } break;

    case WM_NCLBUTTONDOWN:
      // if (msg->wParam == HTCAPTION && d->isState(DraggingInactive)) {
      //   ADS_PRINT("CFloatingDockContainer::nativeEvent WM_NCLBUTTONDOWN");
      //   d->DragStartPos = pos();
      //   d->setState(DraggingMousePressed);
      // }
      break;

    case WM_NCLBUTTONDBLCLK:
      // d->setState(DraggingInactive);
      break;

    case WM_ENTERSIZEMOVE:
      // if (d->isState(DraggingMousePressed)) {
      //   ADS_PRINT("CFloatingDockContainer::nativeEvent WM_ENTERSIZEMOVE");
      //   d->setState(DraggingFloatingWidget);
      //   d->updateDropOverlays(QCursor::pos());
      // }
      break;

    case WM_EXITSIZEMOVE:
      // if (d->isState(DraggingFloatingWidget)) {
      //   ADS_PRINT("CFloatingDockContainer::nativeEvent WM_EXITSIZEMOVE");
      //   if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
      //     d->handleEscapeKey();
      //   } else {
      //     d->titleMouseReleaseEvent();
      //   }
      // }
      break;
  }
  return false;
}
