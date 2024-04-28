#ifndef WIDGETTAB_H
#define WIDGETTAB_H

#include <QAbstractButton>
#include <QObject>

class WidgetTabPrivate;
class IWidget;
class TabInfoStruct;
class QPushButton;
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

  void setCanspliting(bool can_spliting);
  bool isCanSpliting();

 protected:
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void mouseReleaseEvent(QMouseEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void contextMenuEvent(QContextMenuEvent* e) override;
  virtual void paintEvent(QPaintEvent* e) override = 0;

 Q_SIGNALS:
  void activeTabChanged();
  void closed();
  void tabsplit();
  void closeOtherTabs();
  void moved(const QPoint& global_pos);
};

class WidgetTab : public IWidgetTab {
  Q_OBJECT

 public:
  using Super = IWidgetTab;
  WidgetTab(TabInfoStruct* tab, QWidget* parent = nullptr);
  ~WidgetTab();

  void setCanClosed(bool can_closed);
  bool isCanClosed();

 protected:
  virtual void paintEvent(QPaintEvent* e) override;

 private:
  QPushButton* m_close_btn{nullptr};
};

#endif  // WIDGETTAB_H
