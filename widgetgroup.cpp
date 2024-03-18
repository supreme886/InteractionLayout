#include "widgetgroup.h"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTabWidget>

#include "iwidget.h"
#include "widgettab.h"
#include "widgettabbar.h"

struct WidgetGroupPrivate {
  WidgetGroupPrivate(WidgetGroup *_t)
      : _this(_t),
        m_tabBar(new WidgetTabBar(_this)),
        m_StackedWidget(new QStackedWidget(_this)) {
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom, _this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(m_tabBar);
    l->addWidget(m_StackedWidget);
  }

  WidgetGroup *_this{nullptr};
  WidgetTabBar *m_tabBar{nullptr};
  QStackedWidget *m_StackedWidget{nullptr};
  QScrollArea *m_ScrollArea{nullptr};
};

WidgetGroup::WidgetGroup(QWidget *parent)
    : Supre{parent}, d(new WidgetGroupPrivate(this)) {}

int WidgetGroup::addWidget(QWidget *w) { return insertWidget(0, w); }

int WidgetGroup::insertWidget(int index, QWidget *w) {
  IWidget *iw = qobject_cast<IWidget *>(w);
  if (iw) {
    d->m_tabBar->addWidgetTab(iw->getWidgetTab());
  } else {
    WidgetTab *tab = new WidgetTab;
    d->m_tabBar->addWidgetTab(tab);
  }
  d->m_StackedWidget->insertWidget(index, w);
  d->m_StackedWidget->setCurrentWidget(w);
  return true;
}

void WidgetGroup::setCurrentIndex(int index) {
  d->m_StackedWidget->setCurrentIndex(index);
}

void WidgetGroup::setCurrentWidget(QWidget *widget) {
  d->m_StackedWidget->setCurrentWidget(widget);
}

void WidgetGroup::removeWidgetByIndex(int index) {
  d->m_StackedWidget->removeWidget(d->m_StackedWidget->widget(index));
}

void WidgetGroup::removeWidget(QWidget *widget) {
  d->m_StackedWidget->removeWidget(widget);
}

void WidgetGroup::startSplits() { BaseWidget::startSplits(); }
