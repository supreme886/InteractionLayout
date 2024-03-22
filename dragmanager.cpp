#include "dragmanager.h"

#include <QDebug>
#include <QPointer>
#include <QWidget>

#include "floatingwidgetcontainer.h"
#include "interactionlayout.h"

struct DragManagerPrivate {
  DragManagerPrivate(DragManager *t) : _this(t) {}

  DragManager *_this{nullptr};
  QList<QWidget *> hasDragFeatureWidgets;
  QList<FloatingWidgetContainer *> floatingwidgets;
  int m_DragDistance{6};
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
  QList<QWidget *> widget_list;
  foreach (QWidget *item, d->hasDragFeatureWidgets) {
    InteractionLayout *layout =
        qobject_cast<InteractionLayout *>(item->layout());
    if (layout) {
      if (item->geometry().contains(pos)) {
        layout->hover(w, pos);
      } else {
        layout->setGapIndicatorHide();
      }
    }
  }
}

bool DragManager::widgetPlug(QWidget *w, const QPoint &pos) {
  foreach (QWidget *item, d->hasDragFeatureWidgets) {
    if (item->geometry().contains(pos)) {
      InteractionLayout *layout =
          qobject_cast<InteractionLayout *>(item->layout());
      if (layout) {
        layout->plug(w, pos);
        return true;
      }
    }
  }
  return false;
}

void DragManager::updateDragFeaturesWidgetList() {
  // foreach (auto p, d->hasDragFeatureWidgets) {
  //   qDebug() << Q_FUNC_INFO << p.isNull();
  //   if (p.isNull()) d->hasDragFeatureWidgets.removeAll(p);
  // }
  qDebug() << Q_FUNC_INFO << d->hasDragFeatureWidgets.size();
}
