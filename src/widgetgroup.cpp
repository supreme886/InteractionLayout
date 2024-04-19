#include "widgetgroup.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSharedPointer>
#include <QStackedLayout>
#include <QTabWidget>

#include "dragmanager.h"
#include "global.h"
#include "interface.h"
#include "widgettab.h"
#include "widgettabbar.h"

struct WidgetGroupPrivate {
  WidgetGroupPrivate(WidgetGroup *_t)
      : m_this(_t),
        m_tab_bar(new WidgetTabBar(m_this)),
        m_stacked(new QStackedLayout) {
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom, m_this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(m_tab_bar);
    l->addLayout(m_stacked);
  }

  WidgetGroup *m_this{nullptr};
  IWidgetTabBar *m_tab_bar{nullptr};
  QStackedLayout *m_stacked{nullptr};
  QScrollArea *m_scroll_area{nullptr};
};

WidgetGroup::WidgetGroup(QWidget *parent)
    : Supre{parent}, d(new WidgetGroupPrivate(this)) {}

// QList<TabInfoStruct *> WidgetGroup::getTabInfoStructList() {
//   return d
// }

int WidgetGroup::addWidget(QWidget *w) { return insertWidget(0, w); }

int WidgetGroup::insertWidget(int index, QWidget *w) {
  BaseSubWidget *iw = dynamic_cast<BaseSubWidget *>(w);
  WidgetGroup *gw = dynamic_cast<WidgetGroup *>(w);
  if (gw) {
    d->m_tab_bar->addWidgetTab(gw->getTabInfoStruct());
  } else if (iw) {
    d->m_tab_bar->addWidgetTab(iw->getTabInfoStruct());
  } else {
    d->m_tab_bar->addWidgetTab(new TabInfoStruct(nullptr));
  }
  d->m_stacked->insertWidget(index, w);
  d->m_stacked->setCurrentWidget(w);
  return true;
}

void WidgetGroup::setCurrentIndex(int index) {
  d->m_stacked->setCurrentIndex(index);
}

void WidgetGroup::setCurrentWidget(QWidget *widget) {
  d->m_stacked->setCurrentWidget(widget);
}

void WidgetGroup::removeWidgetByIndex(int index) {
  if (index >= 0 && index < d->m_stacked->count())
    d->m_stacked->takeAt(index)->widget()->deleteLater();
}

void WidgetGroup::removeWidget(QWidget *widget) {
  d->m_stacked->removeWidget(widget);
}

int WidgetGroup::count() { return d->m_stacked->count(); }

void WidgetGroup::startSplits() { BaseSubWidget::startSplits(); }
