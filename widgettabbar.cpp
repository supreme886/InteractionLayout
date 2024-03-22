#include "widgettabbar.h"

#include <QBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollArea>
#include <QToolButton>

#include "global.h"
#include "widgetgroup.h"
#include "widgettab.h"

struct WidgetTabBarPrivate {
  WidgetTabBarPrivate(WidgetTabBar *pubThis, WidgetGroup *wg);

  WidgetTabBar *_this{nullptr};
  WidgetGroup *m_widgetGroup{nullptr};
  QFrame *m_tabListFrame{nullptr};
  QBoxLayout *m_tabListFrameLayout{nullptr};
  QPoint m_startDragPonit;
  DragState m_dragState{DraggingInactive};
};

WidgetTabBarPrivate::WidgetTabBarPrivate(WidgetTabBar *pubThis, WidgetGroup *wg)
    : _this(pubThis),
      m_widgetGroup(wg),
      m_tabListFrame(new QFrame),
      m_tabListFrameLayout(new QBoxLayout(QBoxLayout::LeftToRight, _this)) {
  QScrollArea *area = new QScrollArea(_this);
  area->setWidgetResizable(true);
  area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  area->setWidget(m_tabListFrame);
  m_tabListFrameLayout->setContentsMargins(0, 0, 0, 0);
  m_tabListFrameLayout->setSpacing(5);
  m_tabListFrameLayout->addWidget(area);
}

WidgetTabBar::WidgetTabBar(WidgetGroup *parent)
    : Super(parent), d(new WidgetTabBarPrivate(this, parent)) {
  setFixedHeight(30);
  setMouseTracking(true);
}

bool WidgetTabBar::addWidgetTab(WidgetTab *tab) {
  return insertWidgetTab(0, tab);
}

bool WidgetTabBar::insertWidgetTab(int index, WidgetTab *tab) {
  if (tab && (index >= 0 || index < d->m_tabListFrameLayout->count())) {
    connect(tab, &WidgetTab::clicked, this, &WidgetTabBar::onTabClicked);
    connect(tab, &WidgetTab::closed, this, &WidgetTabBar::onTabClosed);
    connect(tab, &WidgetTab::tabsplit, this, &WidgetTabBar::onTabSplit);
    connect(tab, &WidgetTab::closeOtherTabs, this,
            &WidgetTabBar::onTabCloseOtherTabs);
    connect(tab, &WidgetTab::activeTabChanged, this,
            &WidgetTabBar::onTabActiveTabChanged);
    connect(tab, &WidgetTab::moved, this, &WidgetTabBar::onTabMoved);
    d->m_tabListFrameLayout->insertWidget(index, tab);
    return true;
  }
  return false;
}

void WidgetTabBar::setCurrentIndex(int index) {
  d->m_widgetGroup->setCurrentIndex(index);
}

void WidgetTabBar::setCurrentWidget(QWidget *widget) {
  d->m_widgetGroup->setCurrentWidget(widget);
}

void WidgetTabBar::mousePressEvent(QMouseEvent *event) {
  if (Qt::LeftButton == event->button()) {
    event->accept();
    d->m_startDragPonit = event->pos();
    d->m_dragState = DraggingMousePressed;

    return;
  }

  Super::mousePressEvent(event);
}

void WidgetTabBar::mouseMoveEvent(QMouseEvent *event) {
  Super::mouseMoveEvent(event);
  if (d->m_dragState == DraggingInactive) {
    return;
  }

  int DragDistance = (d->m_startDragPonit - event->pos()).manhattanLength();
  if (DragDistance >= qApp->startDragDistance()) {
    splitWidgt();
    d->m_dragState = DraggingInactive;
  }
}

void WidgetTabBar::mouseReleaseEvent(QMouseEvent *event) {
  if (Qt::LeftButton == event->button()) {
    event->accept();
    auto CurrentDragState = d->m_dragState;
    d->m_startDragPonit = QPoint();
    d->m_dragState = DraggingInactive;
    if (DraggingFloatingWidget == CurrentDragState) {
    }

    return;
  }
  Super::mouseReleaseEvent(event);
}

void WidgetTabBar::mouseDoubleClickEvent(QMouseEvent *event) {}

void WidgetTabBar::onTabActiveTabChanged() {}

void WidgetTabBar::onTabClicked() {
  WidgetTab *tab = qobject_cast<WidgetTab *>(sender());
  if (!tab) {
    return;
  }

  int index = d->m_tabListFrameLayout->indexOf(tab);
  if (index < 0) {
    return;
  }
  setCurrentIndex(index);
}

void WidgetTabBar::onTabClosed() {
  WidgetTab *tab = qobject_cast<WidgetTab *>(sender());
  if (!tab) {
    return;
  }
  int index = d->m_tabListFrameLayout->indexOf(tab);
  if (index < 0) {
    return;
  }

  d->m_tabListFrameLayout->takeAt(index);
  tab->setParent(nullptr);
  d->m_tabListFrameLayout->update();
  d->m_widgetGroup->removeWidgetByIndex(index);
}

void WidgetTabBar::onTabSplit() {
  WidgetTab *tab = qobject_cast<WidgetTab *>(sender());
  if (!tab) {
    return;
  }
  int index = d->m_tabListFrameLayout->indexOf(tab);
  if (index < 0) {
    return;
  }

  d->m_tabListFrameLayout->takeAt(index);
  tab->setParent(nullptr);
  d->m_tabListFrameLayout->update();
}

void WidgetTabBar::onTabCloseOtherTabs() {}

void WidgetTabBar::onTabMoved(const QPoint &GlobalPos) {}

void WidgetTabBar::splitWidgt() {
  if (d->m_widgetGroup) {
    d->m_widgetGroup->startSplits();
  }
}
