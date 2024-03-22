#include "widgetgroup.h"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QStackedLayout>
#include <QTabWidget>

#include "iwidget.h"
#include "widgettab.h"
#include "widgettabbar.h"

struct WidgetGroupPrivate {
  WidgetGroupPrivate(WidgetGroup *_t)
      : _this(_t),
        m_tabBar(new WidgetTabBar(_this)),
        m_Stacked(new QStackedLayout) {
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom, _this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(m_tabBar);
    l->addLayout(m_Stacked);
  }

  WidgetGroup *_this{nullptr};
  WidgetTabBar *m_tabBar{nullptr};
  QStackedLayout *m_Stacked{nullptr};
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
  d->m_Stacked->insertWidget(index, w);
  d->m_Stacked->setCurrentWidget(w);
  return true;
}

void WidgetGroup::setCurrentIndex(int index) {
  d->m_Stacked->setCurrentIndex(index);
}

void WidgetGroup::setCurrentWidget(QWidget *widget) {
  d->m_Stacked->setCurrentWidget(widget);
}

void WidgetGroup::removeWidgetByIndex(int index) {
  if (index >= 0 && index < d->m_Stacked->count())
    d->m_Stacked->takeAt(index)->widget()->deleteLater();
}

void WidgetGroup::removeWidget(QWidget *widget) {
  d->m_Stacked->removeWidget(widget);
}

void WidgetGroup::startSplits() { BaseWidget::startSplits(); }
