#ifndef WIDGETTAB_H
#define WIDGETTAB_H

#include <QAbstractButton>
#include <QObject>

class WidgetTabPrivate;
class IWidget;
class TabInfoStruct;

class IWidgetTab : public QAbstractButton {
  Q_OBJECT
 private:
  WidgetTabPrivate* d;

 public:
  using Super = QAbstractButton;
  IWidgetTab(TabInfoStruct* tab, QWidget* parent = nullptr);
  ~IWidgetTab();

  QIcon getIcon();
  QString titleName();

  void setCanspliting(bool canSpliting);
  bool isCanSpliting();

 protected:
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void mouseReleaseEvent(QMouseEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void contextMenuEvent(QContextMenuEvent* e) override;
  virtual void paintEvent(QPaintEvent* e) override = 0;

 Q_SIGNALS:
  void activeTabChanged();
  void clicked();
  void closed();
  void tabsplit();
  void closeOtherTabs();
  void moved(const QPoint& GlobalPos);
};

class WidgetTab : public IWidgetTab {
  Q_OBJECT

 public:
  using Super = IWidgetTab;
  WidgetTab(TabInfoStruct* tab, QWidget* parent = nullptr);
  ~WidgetTab();

 protected:
  virtual void paintEvent(QPaintEvent* e) override;
};

#endif  // WIDGETTAB_H
