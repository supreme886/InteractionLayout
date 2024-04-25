#include "dragmanager.h"

#include <QDebug>
#include <QPointer>
#include <QSharedPointer>
#include <QWidget>

#include "drageventfilter.h"
#include "floatingwidgetcontainer.h"
#include "interactionlayout.h"

struct DragManagerPrivate {
  DragManagerPrivate(DragManager *t) : _this(t) {
    m_dragfilter = QSharedPointer<DragEventFilter>(new DragEventFilter);
  }

  DragManager *_this{nullptr};
  QList<QWidget *> hasDragFeatureWidgets;
  QList<FloatingWidgetContainer *> floatingwidgets;
  int m_DragDistance{6};
  QSharedPointer<DragEventFilter> m_dragfilter;
  bool m_can_popUp{true};
};

DragManager::DragManager(Supre *parent)
    : Supre{parent}, d(new DragManagerPrivate(this)) {}

void DragManager::addDragFeaturesWidget(QWidget *w) {
  if (w) d->hasDragFeatureWidgets.append(w);
}

void DragManager::addFloatingWidgetContainerWidget(FloatingWidgetContainer *w) {
  if (w) d->floatingwidgets.append(w);
}

void DragManager::moveAndHover(QWidget *w, const QPoint &pos) {
  foreach (QWidget *item, d->hasDragFeatureWidgets) {
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
  foreach (QWidget *item, d->hasDragFeatureWidgets) {
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
  d->m_dragfilter->setDragWidget(w);
}

void DragManager::updateDragFeaturesWidgetList() {}
