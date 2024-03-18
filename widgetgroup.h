#ifndef WIDGETGROUP_H
#define WIDGETGROUP_H

#include <QObject>
#include <QWidget>

#include "basewidget.h"

class WidgetGroupPrivate;

class WidgetGroup : public BaseWidget {
  Q_OBJECT
  WidgetGroupPrivate *d;

 public:
  using Supre = BaseWidget;
  explicit WidgetGroup(QWidget *parent = nullptr);

  int addWidget(QWidget *w);
  int insertWidget(int index, QWidget *w);
  void setCurrentIndex(int index);
  void setCurrentWidget(QWidget *widget);
  void removeWidgetByIndex(int index);
  void removeWidget(QWidget *widget);

  void startSplits();

 signals:
};

#endif  // WIDGETGROUP_H
