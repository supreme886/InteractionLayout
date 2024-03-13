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
  FloatingWidgetContainer(QWidget *w);

 protected:
#ifdef Q_OS_WIN
  bool nativeEvent(const QByteArray &eventType, void *message,
                   long *result) override;
#endif
};

#endif  // FLOATINGWIDGETCONTAINER_H
