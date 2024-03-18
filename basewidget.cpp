#include "basewidget.h"

#include <QEvent>

#include "floatingwidgetcontainer.h"
#include "interactionlayout.h"
#include "widgettab.h"

struct BaseWidgetPrivate {
  BaseWidgetPrivate(BaseWidget *pub)
      : _this(pub) /*, m_widgetTab(new WidgetTab(_this)) */ {}

  BaseWidget *_this{nullptr};
  WidgetTab *m_widgetTab{nullptr};
};

BaseWidget::BaseWidget(QWidget *parent)
    : Super(parent), d(new BaseWidgetPrivate(this)) {}

void BaseWidget::startSplits() {
  QRect r = geometry();
  r.moveTopLeft(mapToGlobal(QPoint(0, 30)));

  QWidget *pw = qobject_cast<QWidget *>(parent());
  if (pw) {
    InteractionLayout *layout = qobject_cast<InteractionLayout *>(pw->layout());
    if (layout) {
      layout->unplug(this);
    }
  }

  FloatingWidgetContainer *container = new FloatingWidgetContainer(this);
  container->setGeometry(r);
  container->show();
  // container->grabMouse();
}

void BaseWidget::endSplits() {}

bool BaseWidget::eventFilter(QObject *watched, QEvent *event) {
  return Super::eventFilter(watched, event);
}

bool BaseWidget::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::WindowTitleChange:
      d->m_widgetTab->setTitleName(windowTitle());
      break;

    case QEvent::MouseButtonDblClick:
      startSplits();
      break;
    default:
      break;
  }
  return Super::event(event);
}

// void BaseWidget::paintEvent(QPaintEvent *event) { Super::paintEvent(event); }
