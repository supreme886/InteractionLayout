#include "widgetresizehandler.h"

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QDesktopWidget>
#include <QEvent>
#include <QFrame>
#include <QMouseEvent>

#define RANGE 4

static QList<MousePosition> getMousePositionByArea(int area) {
  QList<MousePosition> postitionList;
  switch (area) {
    case Left_Area:
      postitionList << Right << Top;
      break;
    case Right_Area:
      postitionList << Left << Top;
      break;
    case Top_Area:
      postitionList << Right << Top;
      break;
    case Bottom_Area:
      postitionList << Right << Top;
      break;
    default:
      break;
  }
};

QSize qSmartMinSize(const QWidget *w) {
  const QSize &sizeHint = w->sizeHint();
  const QSize &minSizeHint = w->minimumSizeHint();
  const QSize &minSize = w->minimumSize();
  const QSize &max_size = w->maximumSize();
  const QSizePolicy &sizePolicy = w->sizePolicy();
  QSize s(0, 0);

  if (sizePolicy.horizontalPolicy() != QSizePolicy::Ignored) {
    if (sizePolicy.horizontalPolicy() & QSizePolicy::ShrinkFlag)
      s.setWidth(minSizeHint.width());
    else
      s.setWidth(qMax(sizeHint.width(), minSizeHint.width()));
  }

  if (sizePolicy.verticalPolicy() != QSizePolicy::Ignored) {
    if (sizePolicy.verticalPolicy() & QSizePolicy::ShrinkFlag) {
      s.setHeight(minSizeHint.height());
    } else {
      s.setHeight(qMax(sizeHint.height(), minSizeHint.height()));
    }
  }

  s = s.boundedTo(max_size);
  if (minSize.width() > 0) s.setWidth(minSize.width());
  if (minSize.height() > 0) s.setHeight(minSize.height());

  return s.expandedTo(QSize(0, 0));
}

bool WidgetResizeHandler::m_resize_horizontal_directionFixed = false;
bool WidgetResizeHandler::m_resize_vertical_directionFixed = false;
WidgetResizeHandler::WidgetResizeHandler(QWidget *parent, QWidget *cw)
    : QObject(parent),
      widget(parent),
      m_child_widget(cw ? cw : parent),
      m_fw(0),
      m_extrahei(0),
      m_button_down(false),
      m_move_resize_mode(false),
      m_size_protect(true),
      m_moving_enabled(true) {
  m_mode = Nowhere;
  QFrame *frame = qobject_cast<QFrame *>(widget);
  m_range = frame ? frame->frameWidth() : RANGE;
  m_range = qMax(RANGE, m_range);
  m_active_for_move = m_active_for_resize = true;
  widget->installEventFilter(this);
}

void WidgetResizeHandler::setActive(Action ac, bool b) {
  if (ac & Move) m_active_for_move = b;
  if (ac & Resize) m_active_for_resize = b;

  if (!isActive()) setMouseCursor(Nowhere);
}

bool WidgetResizeHandler::isActive(Action ac) const {
  bool b = false;
  if (ac & Move) b = m_active_for_move;
  if (ac & Resize) b |= m_active_for_resize;

  return b;
}

bool WidgetResizeHandler::eventFilter(QObject *o, QEvent *ee) {
  if (!isActive() ||
      (ee->type() != QEvent::MouseButtonPress &&
       ee->type() != QEvent::MouseButtonRelease &&
       ee->type() != QEvent::MouseMove && ee->type() != QEvent::KeyPress &&
       ee->type() != QEvent::ShortcutOverride))
    return false;

  Q_ASSERT(o == widget);
  QWidget *w = widget;
  if (QApplication::activePopupWidget()) {
    if (m_button_down && ee->type() == QEvent::MouseButtonRelease)
      m_button_down = false;
    return false;
  }
  switch (ee->type()) {
    case QEvent::MouseButtonPress: {
      QMouseEvent *e = static_cast<QMouseEvent *>(ee);
      if (w->isMaximized()) break;
      const QRect widget_rect = widget->rect().marginsAdded(
          QMargins(m_range, m_range, m_range, m_range));
      const QPoint cursor_point = widget->mapFromGlobal(e->globalPos());
      if (!widget_rect.contains(cursor_point)) return false;
      if (e->button() == Qt::LeftButton) {
        m_button_down = false;
        emit activate();
        bool me = m_moving_enabled;
        m_moving_enabled = (me && o == widget);
        mouseMoveEvent(e);
        m_moving_enabled = me;
        m_button_down = true;
        m_move_offset = widget->mapFromGlobal(e->globalPos());
        m_inverted_move_offset = widget->rect().bottomRight() - m_move_offset;
        if (m_mode != Center && m_mode != Nowhere) {
          widget->grabMouse();
        }
        if (m_mode == Center) {
          if (m_moving_enabled) return true;
        } else {
          return true;
        }
      }
    } break;
    case QEvent::MouseButtonRelease:
      if (w->isMaximized()) break;
      if (static_cast<QMouseEvent *>(ee)->button() == Qt::LeftButton) {
        m_move_resize_mode = false;
        m_button_down = false;
        widget->releaseMouse();
        widget->releaseKeyboard();
        if (m_mode == Center) {
          if (m_moving_enabled) return true;
        } else {
          return true;
        }
      }
      break;
    case QEvent::MouseMove: {
      if (w->isMaximized()) break;
      QMouseEvent *e = static_cast<QMouseEvent *>(ee);
      m_button_down =
          m_button_down &&
          (e->buttons() & Qt::LeftButton);  // safety, state machine broken!
      bool me = m_moving_enabled;
      m_moving_enabled =
          (me && o == widget && (m_button_down || m_move_resize_mode));
      mouseMoveEvent(e);
      m_moving_enabled = me;
      if (m_mode == Center) {
        if (m_moving_enabled) return true;
      } else {
        return true;
      }
    } break;
    case QEvent::Leave: {
      widget->releaseMouse();
      widget->setCursor(Qt::ArrowCursor);
    } break;
    // case QEvent::KeyPress:
    //   keyPressEvent(static_cast<QKeyEvent *>(ee));
    //   break;
    case QEvent::ShortcutOverride:
      m_button_down &=
          ((QGuiApplication::mouseButtons() & Qt::LeftButton) != Qt::NoButton);
      if (m_button_down) {
        ee->accept();
        return true;
      }
      break;
    default:
      break;
  }

  return false;
}

void WidgetResizeHandler::mouseMoveEvent(QMouseEvent *e) {
  QPoint pos = widget->mapFromGlobal(e->globalPos());
  if (!m_move_resize_mode && !m_button_down) {
    if (pos.y() <= m_range && pos.x() <= m_range)
      m_mode = TopLeft;
    else if (pos.y() >= widget->height() - m_range &&
             pos.x() >= widget->width() - m_range)
      m_mode = BottomRight;
    else if (pos.y() >= widget->height() - m_range && pos.x() <= m_range)
      m_mode = BottomLeft;
    else if (pos.y() <= m_range && pos.x() >= widget->width() - m_range)
      m_mode = TopRight;
    else if (pos.y() <= m_range)
      m_mode = Top;
    else if (pos.y() >= widget->height() - m_range)
      m_mode = Bottom;
    else if (pos.x() <= m_range)
      m_mode = Left;
    else if (pos.x() >= widget->width() - m_range)
      m_mode = Right;
    else if (widget->rect().contains(pos))
      m_mode = Center;
    else
      m_mode = Nowhere;

    QVariant area = widget->property("MousePosition");
    if (area.isValid() && area.toInt() != m_mode) {
      m_mode = Nowhere;
    }

    if (widget->isMinimized() || !isActive(Resize)) m_mode = Center;
    setMouseCursor(m_mode);
    return;
  }

  if (m_mode == Center && !m_moving_enabled) return;

  if (widget->testAttribute(Qt::WA_WState_ConfigPending)) return;

  QPoint global_pos =
      (!widget->isWindow() && widget->parentWidget())
          ? widget->parentWidget()->mapFromGlobal(e->globalPos())
          : e->globalPos();
  if (!widget->isWindow() &&
      !widget->parentWidget()->rect().contains(global_pos)) {
    if (global_pos.x() < 0) global_pos.rx() = 0;
    if (global_pos.y() < 0) global_pos.ry() = 0;
    if (m_size_protect && global_pos.x() > widget->parentWidget()->width())
      global_pos.rx() = widget->parentWidget()->width();
    if (m_size_protect && global_pos.y() > widget->parentWidget()->height())
      global_pos.ry() = widget->parentWidget()->height();
  }

  QPoint p = global_pos + m_inverted_move_offset;
  QPoint pp = global_pos - m_move_offset;

  // Workaround for window managers which refuse to move a tool window partially
  // offscreen.
  // if (QGuiApplication::platformName() == QLatin1String("xcb")) {
  //   const QRect desktop = QDesktopWidgetPrivate::availableGeometry(widget);
  //   pp.rx() = qMax(pp.x(), desktop.left());
  //   pp.ry() = qMax(pp.y(), desktop.top());
  //   p.rx() = qMin(p.x(), desktop.right());
  //   p.ry() = qMin(p.y(), desktop.bottom());
  // }

  QSize ms = qSmartMinSize(m_child_widget);
  int mw = ms.width();
  int mh = ms.height();
  if (m_child_widget != widget) {
    mw += 2 * m_fw;
    mh += 2 * m_fw + m_extrahei;
  }

  QSize max_size(m_child_widget->maximumSize());
  if (m_child_widget != widget)
    max_size += QSize(2 * m_fw, 2 * m_fw + m_extrahei);
  QSize mpsize(widget->geometry().right() - pp.x() + 1,
               widget->geometry().bottom() - pp.y() + 1);
  mpsize = mpsize.expandedTo(widget->minimumSize())
               .expandedTo(QSize(mw, mh))
               .boundedTo(max_size);
  QPoint mp(widget->geometry().right() - mpsize.width() + 1,
            widget->geometry().bottom() - mpsize.height() + 1);

  QRect geom = widget->geometry();

  switch (m_mode) {
    case TopLeft:
      geom = QRect(mp, widget->geometry().bottomRight());
      break;
    case BottomRight:
      geom = QRect(widget->geometry().topLeft(), p);
      break;
    case BottomLeft:
      geom = QRect(QPoint(mp.x(), widget->geometry().y()),
                   QPoint(widget->geometry().right(), p.y()));
      break;
    case TopRight:
      geom = QRect(QPoint(widget->geometry().x(), mp.y()),
                   QPoint(p.x(), widget->geometry().bottom()));
      break;
    case Top:
      geom = QRect(QPoint(widget->geometry().left(), mp.y()),
                   widget->geometry().bottomRight());
      break;
    case Bottom:
      geom = QRect(widget->geometry().topLeft(),
                   QPoint(widget->geometry().right(), p.y()));
      break;
    case Left:
      geom = QRect(QPoint(mp.x(), widget->geometry().top()),
                   widget->geometry().bottomRight());
      break;
    case Right:
      geom = QRect(widget->geometry().topLeft(),
                   QPoint(p.x(), widget->geometry().bottom()));
      break;
    case Center:
      geom.moveTopLeft(pp);
      break;
    default:
      break;
  }

  geom = QRect(geom.topLeft(), geom.size()
                                   .expandedTo(widget->minimumSize())
                                   .expandedTo(QSize(mw, mh))
                                   .boundedTo(max_size));

  if (geom != widget->geometry() &&
      (widget->isWindow() || widget->parentWidget()->rect().intersects(geom))) {
    if (m_mode == Center)
      widget->move(geom.topLeft());
    else
      widget->setGeometry(geom);
  }
}

void WidgetResizeHandler::setMouseCursor(MousePosition m) {
  // QObjectList children = widget->children();
  // for (int i = 0; i < children.size(); ++i) {
  //   if (QWidget *w = qobject_cast<QWidget *>(children.at(i))) {
  //     if (!w->testAttribute(Qt::WA_SetCursor)) {
  //       w->setCursor(Qt::ArrowCursor);
  //     }
  //   }
  // }
  switch (m) {
    case TopLeft:
    case BottomRight:
      widget->setCursor(Qt::SizeFDiagCursor);
      break;
    case BottomLeft:
    case TopRight:
      widget->setCursor(Qt::SizeBDiagCursor);
      break;
    case Top:
    case Bottom:
      widget->setCursor(Qt::SizeVerCursor);
      break;
    case Left:
    case Right:
      widget->setCursor(Qt::SizeHorCursor);
      break;
    default:
      widget->setCursor(Qt::ArrowCursor);
      break;
  }
}

// void WidgetResizeHandler::keyPressEvent(QKeyEvent *e) {
//   if (!isMove() && !isResize()) return;
//   bool is_control = e->modifiers() & Qt::ControlModifier;
//   int delta = is_control ? 1 : 8;
//   QPoint pos = QCursor::pos();
//   switch (e->key()) {
//     case Qt::Key_Left:
//       pos.rx() -= delta;
//       if (pos.x() <= QDesktopWidgetPrivate::geometry().left()) {
//         if (mode == TopLeft || mode == BottomLeft) {
//           m_move_offset.rx() += delta;
//           m_inverted_move_offset.rx() += delta;
//         } else {
//           m_move_offset.rx() -= delta;
//           m_inverted_move_offset.rx() -= delta;
//         }
//       }
//       if (isResize() && !resizeHorizontalDirectionFixed) {
//         resizeHorizontalDirectionFixed = true;
//         if (mode == BottomRight)
//           mode = BottomLeft;
//         else if (mode == TopRight)
//           mode = TopLeft;
// #ifndef QT_NO_CURSOR
//         setMouseCursor(mode);
//         widget->grabMouse(widget->cursor());
// #else
//         widget->grabMouse();
// #endif
//       }
//       break;
//     case Qt::Key_Right:
//       pos.rx() += delta;
//       if (pos.x() >= QDesktopWidgetPrivate::geometry().right()) {
//         if (mode == TopRight || mode == BottomRight) {
//           m_move_offset.rx() += delta;
//           m_inverted_move_offset.rx() += delta;
//         } else {
//           m_move_offset.rx() -= delta;
//           m_inverted_move_offset.rx() -= delta;
//         }
//       }
//       if (isResize() && !resizeHorizontalDirectionFixed) {
//         resizeHorizontalDirectionFixed = true;
//         if (mode == BottomLeft)
//           mode = BottomRight;
//         else if (mode == TopLeft)
//           mode = TopRight;
// #ifndef QT_NO_CURSOR
//         setMouseCursor(mode);
//         widget->grabMouse(widget->cursor());
// #else
//         widget->grabMouse();
// #endif
//       }
//       break;
//     case Qt::Key_Up:
//       pos.ry() -= delta;
//       if (pos.y() <= QDesktopWidgetPrivate::geometry().top()) {
//         if (mode == TopLeft || mode == TopRight) {
//           m_move_offset.ry() += delta;
//           m_inverted_move_offset.ry() += delta;
//         } else {
//           m_move_offset.ry() -= delta;
//           m_inverted_move_offset.ry() -= delta;
//         }
//       }
//       if (isResize() && !resizeVerticalDirectionFixed) {
//         resizeVerticalDirectionFixed = true;
//         if (mode == BottomLeft)
//           mode = TopLeft;
//         else if (mode == BottomRight)
//           mode = TopRight;
// #ifndef QT_NO_CURSOR
//         setMouseCursor(mode);
//         widget->grabMouse(widget->cursor());
// #else
//         widget->grabMouse();
// #endif
//       }
//       break;
//     case Qt::Key_Down:
//       pos.ry() += delta;
//       if (pos.y() >= QDesktopWidgetPrivate::geometry().bottom()) {
//         if (mode == BottomLeft || mode == BottomRight) {
//           m_move_offset.ry() += delta;
//           m_inverted_move_offset.ry() += delta;
//         } else {
//           m_move_offset.ry() -= delta;
//           m_inverted_move_offset.ry() -= delta;
//         }
//       }
//       if (isResize() && !resizeVerticalDirectionFixed) {
//         resizeVerticalDirectionFixed = true;
//         if (mode == TopLeft)
//           mode = BottomLeft;
//         else if (mode == TopRight)
//           mode = BottomRight;
// #ifndef QT_NO_CURSOR
//         setMouseCursor(mode);
//         widget->grabMouse(widget->cursor());
// #else
//         widget->grabMouse();
// #endif
//       }
//       break;
//     case Qt::Key_Space:
//     case Qt::Key_Return:
//     case Qt::Key_Enter:
//     case Qt::Key_Escape:
//       m_move_resize_mode = false;
//       widget->releaseMouse();
//       widget->releaseKeyboard();
//       m_button_down = false;
//       break;
//     default:
//       return;
//   }
//   QCursor::setPos(pos);
// }

void WidgetResizeHandler::doResize() {
  if (!m_active_for_resize) return;

  m_move_resize_mode = true;
  m_move_offset = widget->mapFromGlobal(QCursor::pos());
  if (m_move_offset.x() < widget->width() / 2) {
    if (m_move_offset.y() < widget->height() / 2)
      m_mode = TopLeft;
    else
      m_mode = BottomLeft;
  } else {
    if (m_move_offset.y() < widget->height() / 2)
      m_mode = TopRight;
    else
      m_mode = BottomRight;
  }
  m_inverted_move_offset = widget->rect().bottomRight() - m_move_offset;
  setMouseCursor(m_mode);
  widget->grabMouse(widget->cursor());
  widget->grabKeyboard();
  m_resize_horizontal_directionFixed = false;
  m_resize_vertical_directionFixed = false;
}

void WidgetResizeHandler::doMove() {
  if (!m_active_for_move) return;

  m_mode = Center;
  m_move_resize_mode = true;
  m_move_offset = widget->mapFromGlobal(QCursor::pos());
  m_inverted_move_offset = widget->rect().bottomRight() - m_move_offset;
  widget->grabMouse(Qt::SizeAllCursor);
  widget->grabKeyboard();
}
