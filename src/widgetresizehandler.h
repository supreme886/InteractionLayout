#ifndef WIDGETRESIZEHANDLER_H
#define WIDGETRESIZEHANDLER_H

#include <QObject>

#include "global.h"

class QMouseEvent;
class QKeyEvent;

class WidgetResizeHandler : public QObject {
  Q_OBJECT

 public:
  enum Action { Move = 0x01, Resize = 0x02, Any = Move | Resize };

  static bool m_resize_horizontal_directionFixed;
  static bool m_resize_vertical_directionFixed;

  explicit WidgetResizeHandler(QWidget *parent, QWidget *cw = nullptr);
  void setActive(bool b) { setActive(Any, b); }
  void setActive(Action ac, bool b);
  bool isActive() const { return isActive(Any); }
  bool isActive(Action ac) const;
  void setm_moving_enabled(bool b) { m_moving_enabled = b; }
  bool ism_moving_enabled() const { return m_moving_enabled; }

  bool ism_button_down() const { return m_button_down; }

  void setm_extraheight(int h) { m_extrahei = h; }
  void setm_size_protection(bool b) { m_size_protect = b; }

  void setFrameWidth(int w) { m_fw = w; }

  void doResize();
  void doMove();

 Q_SIGNALS:
  void activate();

 protected:
  bool eventFilter(QObject *o, QEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e);
  // void keyPressEvent(QKeyEvent *e);

 private:
  Q_DISABLE_COPY_MOVE(WidgetResizeHandler)

  QWidget *widget;
  QWidget *m_child_widget;
  QPoint m_move_offset;
  QPoint m_inverted_move_offset;
  MousePosition m_mode;
  int m_fw;
  int m_extrahei;
  int m_range;
  uint m_button_down : 1;
  uint m_move_resize_mode : 1;
  uint m_active_for_resize : 1;
  uint m_size_protect : 1;
  uint m_moving_enabled : 1;
  uint m_active_for_move : 1;

  void setMouseCursor(MousePosition m);
  bool isMove() const { return m_move_resize_mode && m_mode == Center; }
  bool isResize() const { return m_move_resize_mode && !isMove(); }
};

#endif  // WIDGETRESIZEHANDLER_H
