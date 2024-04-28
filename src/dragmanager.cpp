#include "dragmanager.h"

#include <QDebug>
#include <QPointer>
#include <QSharedPointer>
#include <QWidget>

#include "drageventfilter.h"
#include "floatingwidgetcontainer.h"
#include "interactionlayout.h"

struct DragManagerPrivate {
  DragManagerPrivate(DragManager *t) : m_this(t) {
    m_drag_filter = QSharedPointer<DragEventFilter>(new DragEventFilter);
  }

  DragManager *m_this{nullptr};
  QList<QWidget *> m_has_drag_feature_widgets;
  QList<FloatingWidgetContainer *> m_floating_widgets;
  int m_drag_distance{6};
  QSharedPointer<DragEventFilter> m_drag_filter;
  bool m_can_popUp{true};
};

DragManager::DragManager(Supre *parent)
    : Supre{parent}, d(new DragManagerPrivate(this)) {}

void DragManager::addDragFeaturesWidget(QWidget *w) {
  if (w) d->m_has_drag_feature_widgets.append(w);
}

void DragManager::addFloatingWidgetContainerWidget(FloatingWidgetContainer *w) {
  if (w) d->m_floating_widgets.append(w);
}

void DragManager::moveAndHover(QWidget *w, const QPoint &pos) {
  foreach (QWidget *item, d->m_has_drag_feature_widgets) {
    if (!item->isVisible()) continue;
    InteractionLayout *layout =
        qobject_cast<InteractionLayout *>(item->layout());
    if (layout) {
      QRect geom(item->mapToGlobal(QPoint(0, 0)), item->size());
      if (geom.contains(pos)) {
        layout->hover(w, pos);
      } else {
        layout->setGapIndicatorHide();
      }
    }
  }
}

bool DragManager::widgetPlug(QWidget *w, const QPoint &pos) {
  foreach (QWidget *item, d->m_has_drag_feature_widgets) {
    QRect geom(item->mapToGlobal(QPoint(0, 0)), item->size());
    if (item->isVisible() && geom.contains(pos)) {
      InteractionLayout *layout =
          dynamic_cast<InteractionLayout *>(item->layout());
      if (layout) {
        return layout->plug(w, pos);
      }
    }
  }
  return false;
}

void DragManager::setDragEventFilter(QWidget *w) {
  d->m_drag_filter->setDragWidget(w);
}

void DragManager::updateDragFeaturesWidgetList() {}
