#ifndef WIDGETRESIZEHANDLER_H
#define WIDGETRESIZEHANDLER_H

#include <QObject>

class QMouseEvent;
class QKeyEvent;

class QWidgetResizeHandler : public QObject {
  Q_OBJECT

 public:
  enum Action { Move = 0x01, Resize = 0x02, Any = Move | Resize };

  explicit QWidgetResizeHandler(QWidget *parent, QWidget *cw = nullptr);
  void setActive(bool b) { setActive(Any, b); }
  void setActive(Action ac, bool b);
  bool isActive() const { return isActive(Any); }
  bool isActive(Action ac) const;
  void setMovingEnabled(bool b) { movingEnabled = b; }
  bool isMovingEnabled() const { return movingEnabled; }

  bool isButtonDown() const { return buttonDown; }

  void setExtraHeight(int h) { extrahei = h; }
  void setSizeProtection(bool b) { sizeprotect = b; }

  void setFrameWidth(int w) { fw = w; }

  void doResize();
  void doMove();

 Q_SIGNALS:
  void activate();

 protected:
  bool eventFilter(QObject *o, QEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e);
  // void keyPressEvent(QKeyEvent *e);

 private:
  Q_DISABLE_COPY_MOVE(QWidgetResizeHandler)

  enum MousePosition {
    Nowhere,
    TopLeft,
    BottomRight,
    BottomLeft,
    TopRight,
    Top,
    Bottom,
    Left,
    Right,
    Center
  };

  QWidget *widget;
  QWidget *childWidget;
  QPoint moveOffset;
  QPoint invertedMoveOffset;
  MousePosition mode;
  int fw;
  int extrahei;
  int range;
  uint buttonDown : 1;
  uint moveResizeMode : 1;
  uint activeForResize : 1;
  uint sizeprotect : 1;
  uint movingEnabled : 1;
  uint activeForMove : 1;

  void setMouseCursor(MousePosition m);
  bool isMove() const { return moveResizeMode && mode == Center; }
  bool isResize() const { return moveResizeMode && !isMove(); }
};

#endif  // WIDGETRESIZEHANDLER_H
