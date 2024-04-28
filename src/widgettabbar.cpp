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
  WidgetTabBarPrivate(WidgetTabBar *pubm_this, WidgetGroup *wg);

  WidgetTabBar *m_this{nullptr};
  WidgetGroup *m_widget_group{nullptr};
  QFrame *m_tab_list_frame{nullptr};
  QBoxLayout *m_tab_list_frame_layout{nullptr};
  QButtonGroup *m_tab_group{nullptr};
  QPoint m_start_drag_ponit;
  DragState m_drag_state{DraggingInactive};
};

WidgetTabBarPrivate::WidgetTabBarPrivate(WidgetTabBar *pubThis, WidgetGroup *wg)
    : m_this(pubThis),
      m_widget_group(wg),
      m_tab_list_frame(new QFrame),
      m_tab_group(new QButtonGroup),
      m_tab_list_frame_layout(new QBoxLayout(QBoxLayout::LeftToRight, m_this)) {
  QScrollArea *area = new QScrollArea(m_this);
  area->setWidgetResizable(true);
  area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  area->setWidget(m_tab_list_frame);
  m_tab_list_frame_layout->setContentsMargins(0, 0, 0, 0);
  m_tab_list_frame_layout->setSpacing(2);
  m_tab_group->setExclusive(true);
  // m_tab_list_frame_layout->addWidget(area);
}

WidgetTabBar::WidgetTabBar(WidgetGroup *parent)
    : Super(parent), d(new WidgetTabBarPrivate(this, parent)) {
  setFixedHeight(30);
  setMouseTracking(true);
}

bool WidgetTabBar::addWidgetTab(TabInfoStruct *tab) {
  return insertWidgetTab(0, tab);
}

bool WidgetTabBar::insertWidgetTab(int index, TabInfoStruct *tab_info) {
  if (tab_info && (index >= 0 || index < d->m_tab_list_frame_layout->count())) {
    WidgetTab *tab = new WidgetTab(tab_info);
    tab->setCanspliting(tab_info->m_can_spliting);
    tab->setCanClosed(tab_info->m_can_closed);
    connect(tab, &IWidgetTab::clicked, this, &WidgetTabBar::onTabClicked);
    connect(tab, &IWidgetTab::closed, this, &WidgetTabBar::onTabClosed);
    connect(tab, &IWidgetTab::tabsplit, this, &WidgetTabBar::onTabSplit);
    connect(tab, &IWidgetTab::closeOtherTabs, this,
            &WidgetTabBar::onTabCloseOtherTabs);
    connect(tab, &IWidgetTab::activeTabChanged, this,
            &WidgetTabBar::onTabActiveTabChanged);
    connect(tab, &IWidgetTab::moved, this, &WidgetTabBar::onTabMoved);
    d->m_tab_list_frame_layout->insertWidget(index, tab);
    d->m_tab_group->addButton(tab);
    return true;
  }
  return false;
}

void WidgetTabBar::setCurrentIndex(int index) {
  d->m_widget_group->setCurrentIndex(index);
}

void WidgetTabBar::setCurrentWidget(QWidget *widget) {
  d->m_widget_group->setCurrentWidget(widget);
}

void WidgetTabBar::mousePressEvent(QMouseEvent *event) {
  if (Qt::LeftButton == event->button() &&
      cursor().shape() == Qt::ArrowCursor) {
    event->accept();
    d->m_start_drag_ponit = event->pos();
    d->m_drag_state = DraggingMousePressed;

    return;
  }

  Super::mousePressEvent(event);
}

void WidgetTabBar::mouseMoveEvent(QMouseEvent *event) {
  Super::mouseMoveEvent(event);
  if (d->m_drag_state == DraggingInactive) {
    return;
  }

  int DragDistance = (d->m_start_drag_ponit - event->pos()).manhattanLength();
  if (DragDistance >= qApp->startDragDistance()) {
    splitWidgt();
    d->m_drag_state = DraggingInactive;
  }
}

void WidgetTabBar::mouseReleaseEvent(QMouseEvent *event) {
  if (Qt::LeftButton == event->button()) {
    event->accept();
    auto CurrentDragState = d->m_drag_state;
    d->m_start_drag_ponit = QPoint();
    d->m_drag_state = DraggingInactive;
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

  int index = d->m_tab_list_frame_layout->indexOf(tab);
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
  int index = d->m_tab_list_frame_layout->indexOf(tab);
  if (index < 0) {
    return;
  }

  d->m_tab_list_frame_layout->takeAt(index);
  d->m_tab_list_frame_layout->update();
  d->m_widget_group->removeWidgetByIndex(index);
  buttonRemove(tab);
  tab->deleteLater();
}

void WidgetTabBar::onTabSplit() {
  IWidgetTab *tab = qobject_cast<IWidgetTab *>(sender());
  if (!tab) {
    return;
  }
  int index = d->m_tab_list_frame_layout->indexOf(tab);
  if (index < 0) {
    return;
  }

  d->m_tab_list_frame_layout->takeAt(index);
  tab->setParent(nullptr);
  buttonRemove(tab);
}

void WidgetTabBar::onTabCloseOtherTabs() {}

void WidgetTabBar::onTabMoved(const QPoint &GlobalPos) {}

void WidgetTabBar::splitWidgt() {
  if (d->m_widget_group) {
    d->m_widget_group->startSplits();
  }
}

void WidgetTabBar::buttonRemove(IWidgetTab *tab) {
  if (!d->m_tab_list_frame_layout->count()) {
    d->m_widget_group->deleteLater();
    return;
  }
  if (tab) d->m_tab_group->removeButton(tab);
  d->m_tab_group->buttons()
      .at(d->m_tab_group->buttons().size() - 1)
      ->setChecked(true);
  d->m_widget_group->setCurrentIndex(0);
}
