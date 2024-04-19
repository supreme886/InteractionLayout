#ifndef DRAGEVENTFILTER_H
#define DRAGEVENTFILTER_H

#include <QObject>

class DragEventFilterPrivate;
class DragEventFilter : public QObject {
  Q_OBJECT
  DragEventFilterPrivate *d;

 public:
  explicit DragEventFilter(QObject *parent = nullptr);

  void setDragWidget(QWidget *);

  bool dragWidgetEmpty();

  void beginDrag(const QPoint &point);

 protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

  virtual void startDrag(QEvent *event);
  virtual void DragMoving(QEvent *event);
  virtual void endDrag(QEvent *event);
};

#endif  // DRAGEVENTFILTER_H
