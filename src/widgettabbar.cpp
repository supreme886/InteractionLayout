#include "widgettabbar.h"

#include <QBoxLayout>
#include <QButtonGroup>
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
  QButtonGroup *m_tabGroup{nullptr};
  QPoint m_startDragPonit;
  DragState m_dragState{DraggingInactive};
};

WidgetTabBarPrivate::WidgetTabBarPrivate(WidgetTabBar *pubThis, WidgetGroup *wg)
    : _this(pubThis),
      m_widgetGroup(wg),
      m_tabListFrame(new QFrame),
      m_tabGroup(new QButtonGroup),
      m_tabListFrameLayout(new QBoxLayout(QBoxLayout::LeftToRight, _this)) {
  QScrollArea *area = new QScrollArea(_this);
  area->setWidgetResizable(true);
  area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  area->setWidget(m_tabListFrame);
  m_tabListFrameLayout->setContentsMargins(0, 0, 0, 0);
  m_tabListFrameLayout->setSpacing(2);
  m_tabGroup->setExclusive(true);
  // m_tabListFrameLayout->addWidget(area);
}

WidgetTabBar::WidgetTabBar(WidgetGroup *parent)
    : Super(parent), d(new WidgetTabBarPrivate(this, parent)) {
  setFixedHeight(30);
  setMouseTracking(true);
}

bool WidgetTabBar::addWidgetTab(TabInfoStruct *tab) {
  return insertWidgetTab(0, tab);
}

bool WidgetTabBar::insertWidgetTab(int index, TabInfoStruct *tabinfo) {
  if (tabinfo && (index >= 0 || index < d->m_tabListFrameLayout->count())) {
    WidgetTab *tab = new WidgetTab(tabinfo);
    connect(tab, &IWidgetTab::clicked, this, &WidgetTabBar::onTabClicked);
    connect(tab, &IWidgetTab::closed, this, &WidgetTabBar::onTabClosed);
    connect(tab, &IWidgetTab::tabsplit, this, &WidgetTabBar::onTabSplit);
    connect(tab, &IWidgetTab::closeOtherTabs, this,
            &WidgetTabBar::onTabCloseOtherTabs);
    connect(tab, &IWidgetTab::activeTabChanged, this,
            &WidgetTabBar::onTabActiveTabChanged);
    connect(tab, &IWidgetTab::moved, this, &WidgetTabBar::onTabMoved);
    d->m_tabListFrameLayout->insertWidget(index, tab);
    d->m_tabGroup->addButton(tab);
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
  IWidgetTab *tab = qobject_cast<IWidgetTab *>(sender());
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
  IWidgetTab *tab = qobject_cast<IWidgetTab *>(sender());
  if (!tab) {
    return;
  }
  int index = d->m_tabListFrameLayout->indexOf(tab);
  if (index < 0) {
    return;
  }

  d->m_tabListFrameLayout->takeAt(index);
  d->m_tabListFrameLayout->update();
  d->m_widgetGroup->removeWidgetByIndex(index);
  buttonRemove(tab);
  tab->deleteLater();
}

void WidgetTabBar::onTabSplit() {
  IWidgetTab *tab = qobject_cast<IWidgetTab *>(sender());
  if (!tab) {
    return;
  }
  int index = d->m_tabListFrameLayout->indexOf(tab);
  if (index < 0) {
    return;
  }

  d->m_tabListFrameLayout->takeAt(index);
  tab->setParent(nullptr);
  buttonRemove(tab);
}

void WidgetTabBar::onTabCloseOtherTabs() {}

void WidgetTabBar::onTabMoved(const QPoint &GlobalPos) {}

void WidgetTabBar::splitWidgt() {
  if (d->m_widgetGroup) {
    d->m_widgetGroup->startSplits();
  }
}

void WidgetTabBar::buttonRemove(IWidgetTab *tab) {
  if (!d->m_tabListFrameLayout->count()) {
    d->m_widgetGroup->deleteLater();
    return;
  }
  if (tab) d->m_tabGroup->removeButton(tab);
  d->m_tabGroup->buttons()
      .at(d->m_tabGroup->buttons().size() - 1)
      ->setChecked(true);
  d->m_widgetGroup->setCurrentIndex(0);
}
