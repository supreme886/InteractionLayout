#ifndef DRAGMANAGER_H
#define DRAGMANAGER_H

#include <QObject>

class DragManagerPrivate;
class FloatingWidgetContainer;
class DragManager : public QObject {
  Q_OBJECT

 public:
  using Supre = QObject;

  static DragManager *instance() {
    static DragManager manager;
    return &manager;
  }

  void addDragFeaturesWidget(QWidget *w);
  void addFloatingWidgetContainerWidget(FloatingWidgetContainer *w);

  void moveAndHover(QWidget *w, const QPoint &pos);

  void widgetPlug(QWidget *w, const QPoint &pos);

 public slots:
  void updateDragFeaturesWidgetList();

 private:
  DragManagerPrivate *d;
  explicit DragManager(Supre *parent = nullptr);
};

#endif  // DRAGMANAGER_H
