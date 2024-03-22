#ifndef WIDGETTABBAR_H
#define WIDGETTABBAR_H

#include <QFrame>
#include <QObject>

class WidgetTabBarPrivate;
class WidgetTab;
class WidgetGroup;

class WidgetTabBar : public QFrame {
  Q_OBJECT

  WidgetTabBarPrivate *d;

 public:
  using Super = QFrame;
  WidgetTabBar(WidgetGroup *parent = nullptr);

  bool addWidgetTab(WidgetTab *tab);
  bool insertWidgetTab(int index, WidgetTab *tab);

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
};

#endif  // WIDGETTABBAR_H
