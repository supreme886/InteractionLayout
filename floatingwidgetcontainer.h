#ifndef FLOATINGWIDGETCONTAINER_H
#define FLOATINGWIDGETCONTAINER_H

#include <QObject>
#include <QWidget>

class WidgetGroup;
class FloatingWidgetContainerPrivate;
class FloatingWidgetContainer : public QWidget {
  Q_OBJECT
  FloatingWidgetContainerPrivate *d;

 public:
  using Supre = QWidget;
  FloatingWidgetContainer(WidgetGroup *wg);

 protected:
  bool nativeEvent(const QByteArray &eventType, void *message,
                   long *result) override;
};

#endif  // FLOATINGWIDGETCONTAINER_H
