#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QFrame>
#include <QObject>

class BaseWidgetPrivate;
class FloatingWidgetContainer;
class BaseWidget : public QFrame {
  Q_OBJECT
  BaseWidgetPrivate *d;

 public:
  using Super = QFrame;
  BaseWidget(QWidget *parent = nullptr);

  FloatingWidgetContainer *startSplits();
  void endSplits();

 protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  bool event(QEvent *event) override;
  // void paintEvent(QPaintEvent *event) override;
};

#endif  // BASEWIDGET_H
