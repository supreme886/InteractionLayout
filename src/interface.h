#ifndef INTERFACE_H
#define INTERFACE_H

#include <QFrame>

class TabInfoStruct;
class IWidgetTabBar : public QFrame {
 public:
  IWidgetTabBar(QWidget *parent) : QFrame(parent) {}

  virtual bool addWidgetTab(TabInfoStruct *tab) = 0;
  virtual bool insertWidgetTab(int index, TabInfoStruct *tab) = 0;

  virtual void setCurrentIndex(int index) = 0;
  virtual void setCurrentWidget(QWidget *widget) = 0;
};
#endif  // INTERFACE_H
