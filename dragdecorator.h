#ifndef DRAGDECORATOR_H
#define DRAGDECORATOR_H

#include <QEvent>
#include <QLayoutItem>
#include <QObject>

template <typename T,
          typename = std::enable_if_t<std::is_base_of<QWidget, T>::value>>
class DragDecorator : public QObject {
 public:
  explicit DragDecorator(T *parent) : QObject(parent) {
    m_Widget = parent;
    m_Widget->setMouseTracking(true);
    m_Widget->installEventFilter(this);
  }

  // void initState(const QPoint &pos) {
  //   if (state != 0) return;
  //   state = new Movestate::Movestate;
  //   state->m_press_point = pos;
  //   state->dragging = false;
  //   state->widgetItem = 0;
  // }

  // void startDrag() {
  //   if (state == 0 || state->dragging) return;

  //   QRect r = m_Widget->geometry();
  //   r.moveTopLeft(m_Widget->mapToGlobal(QPoint(0, 30)));
  //   // setWindowState(true, r);

  //   state->dragging = true;
  // }
  // void endDrag();

  // bool eventFilter(QObject *watched, QEvent *event) {
  //   switch (event->type()) {
  //     case QEvent::Hide:
  //       break;
  //     case QEvent::Show:

  //       break;
  //     case QEvent::ApplicationLayoutDirectionChange:
  //     case QEvent::LayoutDirectionChange:
  //     case QEvent::StyleChange:
  //     case QEvent::ParentChange:
  //       break;
  //     case QEvent::ZOrderChange: {
  //       break;
  //     }
  //     case QEvent::WindowActivate:
  //     case QEvent::WindowDeactivate:
  //       // update(qobject_cast<QDockWidgetLayout
  //       // *>(this->layout())->titleArea());
  //       break;
  //     case QEvent::ContextMenu:
  //       // if (d->state) {
  //       //   event->accept();
  //       //   return true;
  //       // }
  //       break;
  //       // return true after calling the handler since we don't want
  //       // them to be passed onto the default handlers
  //     case QEvent::MouseButtonPress:
  //       if (d->mousePressEvent(static_cast<QMouseEvent *>(event))) return
  //       true; break;
  //     case QEvent::MouseButtonDblClick:
  //       if (d->mouseDoubleClickEvent(static_cast<QMouseEvent *>(event)))
  //         return true;
  //       break;
  //     case QEvent::MouseMove:
  //       if (d->mouseMoveEvent(static_cast<QMouseEvent *>(event))) return
  //       true; break;
  //     case QEvent::MouseButtonRelease:
  //       if (d->mouseReleaseEvent(static_cast<QMouseEvent *>(event)))
  //         return true;
  //       break;
  //     case QEvent::NonClientAreaMouseMove:
  //     case QEvent::NonClientAreaMouseButtonPress:
  //     case QEvent::NonClientAreaMouseButtonRelease:
  //     case QEvent::NonClientAreaMouseButtonDblClick:
  //       d->nonClientAreaMouseEvent(static_cast<QMouseEvent *>(event));
  //       return true;
  //     case QEvent::Move:

  //       break;
  //     case QEvent::Resize:

  //       break;
  //     default:
  //       break;
  //   }
  //   return QWidget::event(event);
  // }

 private:
  T *m_Widget{nullptr};
  struct Movestate {
    bool dragging{false};
    QPoint m_press_point;
    QLayoutItem *widgetItem;
  };

  // QWidgetResizeHandler *resizer{nullptr};
  Movestate *state{nullptr};
};

#endif  // DRAGDECORATOR_H
