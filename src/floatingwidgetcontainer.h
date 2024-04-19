#ifndef FLOATINGWIDGETCONTAINER_H
#define FLOATINGWIDGETCONTAINER_H

#include <QObject>
#include <QWidget>

class FloatingWidgetContainerPrivate;
class FloatingWidgetContainer : public QWidget {
  Q_OBJECT
  FloatingWidgetContainerPrivate *d;

 public:
  using Supre = QWidget;
  FloatingWidgetContainer(QWidget *w = nullptr);

  QWidget *getTargetWidget();

 protected:
  bool event(QEvent *event) override;
#ifdef Q_OS_WIN
  bool nativeEvent(const QByteArray &eventType, void *message,
                   long *result) override;
#endif

 private:
  void setWidgetIntoLayout();
};

#endif  // FLOATINGWIDGETCONTAINER_H
