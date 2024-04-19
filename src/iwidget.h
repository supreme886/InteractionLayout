#ifndef IWIDGET_H
#define IWIDGET_H

#include <QObject>
#include <QWidget>

class IWidgetPrivate;
class IWidgetTab;
class TabInfoStruct;

class IWidget : public QWidget {
  Q_OBJECT
  Q_DECLARE_PRIVATE(IWidget)
 public:
  explicit IWidget(QWidget *parent = nullptr,
                   Qt::WindowFlags flags = Qt::WindowFlags());

  TabInfoStruct *getWidgetTabShared();

  void setFloating(bool floating, const QRect &rect);
  inline bool isFloating() { return isWindow(); };

 protected:
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

 signals:
};

#endif  // IWIDGET_H
