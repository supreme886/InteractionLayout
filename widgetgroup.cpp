#include "widgetgroup.h"

#include <QHBoxLayout>
#include <QTabWidget>

struct WidgetGroupPrivate {
  WidgetGroupPrivate(WidgetGroup *_t) : _this(_t), m_tabbar(new QTabWidget()) {}

  QTabWidget *m_tabbar{nullptr};
  WidgetGroup *_this{nullptr};
};

WidgetGroup::WidgetGroup(Supre *parent)
    : Supre{parent}, d(new WidgetGroupPrivate(this)) {
  QHBoxLayout *l = new QHBoxLayout(this);
  l->setMargin(0);
  l->addWidget(d->m_tabbar);
}

int WidgetGroup::addWidget(QWidget *w) { return insertWidget(0, w); }

int WidgetGroup::insertWidget(int index, QWidget *w) {
  return d->m_tabbar->insertTab(index, w, w->windowTitle());
}

void WidgetGroup::setCurrentIndex(int index) {
  d->m_tabbar->setCurrentIndex(index);
}

void WidgetGroup::setCurrentWidget(QWidget *widget) {
  d->m_tabbar->setCornerWidget(widget);
}
