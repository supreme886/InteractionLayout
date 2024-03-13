#ifndef WIDGETGROUP_H
#define WIDGETGROUP_H

#include <QObject>
#include <QWidget>

class WidgetGroupPrivate;

class WidgetGroup : public QWidget {
  Q_OBJECT
  WidgetGroupPrivate *d;

 public:
  using Supre = QWidget;
  explicit WidgetGroup(Supre *parent = nullptr);

  int addWidget(QWidget *w);
  int insertWidget(int index, QWidget *w);
  void setCurrentIndex(int index);
  void setCurrentWidget(QWidget *widget);

 signals:
};

#endif  // WIDGETGROUP_H
