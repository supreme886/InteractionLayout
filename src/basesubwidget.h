#ifndef BASESUBWIDGET_H
#define BASESUBWIDGET_H

#include <QFrame>
#include <QObject>

class BaseWidgetPrivate;
class FloatingWidgetContainer;
class TabInfoStruct;
class BaseSubWidget : public QFrame {
  Q_OBJECT
  BaseWidgetPrivate *d;

 public:
  using Super = QFrame;
  BaseSubWidget(QWidget *parent = nullptr);
  ~BaseSubWidget();

  FloatingWidgetContainer *startSplits();
  void endSplits();
  void setResizeHandle(bool can_resize);

  TabInfoStruct *getTabInfoStruct();

  void setCanspliting(bool can_spliting);
  bool isCanSpliting();

  void setCanResize(bool can_resize);
  bool isCanResize();

  void setCanClosed(bool can_closed);
  bool isCanClosed();

 protected:
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;

 private:
  void unPlugSelf();
};

#endif  // BASESUBWIDGET_H
