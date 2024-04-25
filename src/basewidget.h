#ifndef BASEWIDGET_H
#define BASEWIDGET_H

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
  void setResizeHandle(bool canResize);

  TabInfoStruct *getTabInfoStruct();

  void setCanspliting(bool canSpliting);
  bool isCanSpliting();

  void setCanResize(bool canResize);
  bool isCanResize();

 protected:
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;

 private:
  void unPlugSelf();
};

#endif  // BASEWIDGET_H
