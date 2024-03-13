#ifndef WIDGETTAB_H
#define WIDGETTAB_H

#include <QFrame>
#include <QObject>

class WidgetTabPrivate;

class WidgetTab : public QFrame {
  Q_OBJECT
 private:
  WidgetTabPrivate* d;

 public:
  using Supre = QFrame;
  WidgetTab(QWidget* tagWidget, QWidget* parent = nullptr);

 protected:
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void mouseReleaseEvent(QMouseEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void contextMenuEvent(QContextMenuEvent* e) override;

 Q_SIGNALS:
  void activeTabChanged();
  void clicked();
  void closed();
  void closeOtherTabs();
  void moved(const QPoint& GlobalPos);
};

#endif  // WIDGETTAB_H
