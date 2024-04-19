#ifndef WIDGETTABBAR_H
#define WIDGETTABBAR_H

#include <QFrame>
#include <QObject>

#include "interface.h"

class WidgetTabBarPrivate;
class IWidgetTab;
class WidgetGroup;
class TabInfoStruct;

class WidgetTabBar : public IWidgetTabBar {
  Q_OBJECT

  WidgetTabBarPrivate *d;

 public:
  using Super = IWidgetTabBar;
  WidgetTabBar(WidgetGroup *parent = nullptr);

  bool addWidgetTab(TabInfoStruct *tab);
  bool insertWidgetTab(int index, TabInfoStruct *tab);

  void setCurrentIndex(int index);
  void setCurrentWidget(QWidget *widget);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

 public Q_SLOTS:
  void onTabActiveTabChanged();
  void onTabClicked();
  void onTabClosed();
  void onTabSplit();
  void onTabCloseOtherTabs();
  void onTabMoved(const QPoint &GlobalPos);

 private:
  void splitWidgt();
  void buttonRemove(IWidgetTab *tab);
};

#endif  // WIDGETTABBAR_H
