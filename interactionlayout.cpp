#include "interactionlayout.h"

#include <private/qlayout_p.h>

#include <QDebug>
#include <QVarLengthArray>
#include <QWidget>

#include "dragmanager.h"
#include "widgetgroup.h"

class InteractionLayoutPrivate : public QLayoutPrivate {
  Q_DECLARE_PUBLIC(InteractionLayout)
 public:
  InteractionLayoutPrivate() {
    m_corner_belong.insert(LayoutInterface::Left_Top_Corner,
                           LayoutInterface::Top_Area);
    m_corner_belong.insert(LayoutInterface::Right_Top_Corner,
                           LayoutInterface::Right_Area);
    m_corner_belong.insert(LayoutInterface::Right_Bottom_Corner,
                           LayoutInterface::Bottom_Area);
    m_corner_belong.insert(LayoutInterface::Left_Bottom_Corner,
                           LayoutInterface::Left_Area);
  }

  void addItem(QLayoutItem *) const;
  QLayoutItem *itemAt(int) const;
  QLayoutItem *takeAt(int) const;
  int count() const;

  mutable QList<QLayoutItem *> m_items;
  QMap<LayoutInterface::Area, QList<QWidget *>> m_widgets_map;
  QMap<LayoutInterface::Corner, LayoutInterface::Area> m_corner_belong;
  int m_hSpace{5};
  int m_vSpace{5};
  QRect m_rect;
  bool m_hover{false};
  QPointer<QRubberBand> gapIndicator;
};

void InteractionLayoutPrivate::addItem(QLayoutItem *it) const {
  m_items.append(it);
}

QLayoutItem *InteractionLayoutPrivate::itemAt(int index) const {
  return m_items.value(index);
}

QLayoutItem *InteractionLayoutPrivate::takeAt(int index) const {
  if (index >= 0 && index < m_items.size()) return m_items.takeAt(index);
  return nullptr;
}

int InteractionLayoutPrivate::count() const { return m_items.size(); }

InteractionLayout::InteractionLayout(QWidget *parent)
    : QLayout(*new InteractionLayoutPrivate(), 0, parent) {
  DragManager::instance()->addDragFeaturesWidget(parent);
}

InteractionLayout::InteractionLayout()
    : QLayout(*new InteractionLayoutPrivate(), 0, 0) {}

InteractionLayout::~InteractionLayout() {}

void InteractionLayout::addItem(QLayoutItem *item) {
  Q_D(const InteractionLayout);
  d->addItem(item);
}

Qt::Orientations InteractionLayout::expandingDirections() const { return 0; }

QSize InteractionLayout::minimumSize() const { return QSize(); }

QSize InteractionLayout::maximumSize() const { return QSize(); }

QSize InteractionLayout::sizeHint() const { return QSize(); }

void InteractionLayout::setGeometry(const QRect &r) {
  Q_D(InteractionLayout);
  if (!count()) return;

  d->m_rect = r;

  QVarLengthArray<QRect, 4> rectList = getRectForArea(d->m_hover);
  // qDebug() << Q_FUNC_INFO << rectList.at(0) << rectList.at(1) <<
  // rectList.at(2)
  // << rectList.at(3);
  for (auto iter = d->m_widgets_map.begin(); iter != d->m_widgets_map.end();
       iter++) {
    if (iter.value().size() && iter.key() < rectList.size()) {
      iter.value().at(0)->setGeometry(rectList.at(iter.key()));
    }
  }

  // int x = lMargin;
  // for (int i = 0; i < count(); i++) {
  //   QLayoutItem *item = itemAt(i);
  //   qAsConst(item);
  //   QWidget *wd = item->widget();
  //   wd->move(x, tMargin);
  //   x += wd->width() + horizontalSpacing();
  // }
}

QLayoutItem *InteractionLayout::itemAt(int index) const {
  Q_D(const InteractionLayout);
  return d->itemAt(index);
}

QLayoutItem *InteractionLayout::takeAt(int index) {
  Q_D(const InteractionLayout);
  return d->takeAt(index);
}

int InteractionLayout::count() const {
  Q_D(const InteractionLayout);
  return d->count();
}

bool InteractionLayout::hasHeightForWidth() const { return false; }

int InteractionLayout::heightForWidth(int i) const { return -1; }

int InteractionLayout::minimumHeightForWidth(int w) const {
  return heightForWidth(w);
}

int InteractionLayout::horizontalSpacing() const {
  Q_D(const InteractionLayout);
  if (d->m_hSpace >= 0) {
    return d->m_hSpace;
  } else {
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
  }
}

int InteractionLayout::verticalSpacing() const {
  Q_D(const InteractionLayout);
  if (d->m_vSpace >= 0) {
    return d->m_vSpace;
  } else {
    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
  }
}

QSize InteractionLayout::closestAcceptableSize(const QWidget *w,
                                               const QSize &s) {
  return QSize();
}

QLayoutItem *InteractionLayout::unplug(QWidget *widget) {
  Q_D(InteractionLayout);
  for (int i = 0; i < count(); i++) {
    QLayoutItem *item = itemAt(i);
    if (widget == item->widget()) return takeAt(i);
  }
  for (auto iter = d->m_widgets_map.begin(); iter != d->m_widgets_map.end();
       iter++) {
    if (iter.value().size() && iter.value().contains(widget)) {
      iter.value().removeAll(widget);
    }
  }
  return nullptr;
}

bool InteractionLayout::plug(QWidget *widget, const QPoint &mousePos) {
  Q_D(InteractionLayout);

  d->m_hover = false;
  QVarLengthArray<QRect, 4> rectList = getRectForArea(d->m_hover);
  int targetArea = -1;
  for (int i = 0; i < 4; i++) {
    if (rectList.at(i).contains(parentWidget()->mapFromGlobal(mousePos))) {
      targetArea = i;
    }
  }

  if (targetArea >= 0 && targetArea < 4) {
    widget->setWindowFlags(widget->windowFlags() & ~Qt::Tool);
    widget->setParent(parentWidget());
    widget->show();
    addWidgetByArea((LayoutInterface::Area)targetArea, widget);
  } /*else {
    if (d->gapIndicator) {
      delete d->gapIndicator;
    }
  }*/

  if (d->gapIndicator) {
    delete d->gapIndicator;
  }

  update();
  return true;
}

void InteractionLayout::hover(QWidget *widget, const QPoint &mousePos,
                              bool isMoving) {
  Q_D(InteractionLayout);

  d->m_hover = false;
  QVarLengthArray<QRect, 4> rectList = getRectForArea(d->m_hover);
  qDebug() << Q_FUNC_INFO;
  int targetArea = -1;
  for (int i = 0; i < 4; i++) {
    qDebug() << Q_FUNC_INFO << rectList.at(i) << mousePos;
    if (rectList.at(i).contains(parentWidget()->mapFromGlobal(mousePos))) {
      targetArea = i;
    }
  }

  if (targetArea >= 0 && targetArea < 4) {
    if (d->gapIndicator.isNull()) {
      d->gapIndicator = new QRubberBand(QRubberBand::Rectangle, parentWidget());
    }
    d->gapIndicator->setGeometry(rectList.at(targetArea));
    d->gapIndicator->show();
    d->gapIndicator->raise();
  } else {
    if (d->gapIndicator) {
      delete d->gapIndicator;
    }
  }

  update();
}

bool InteractionLayout::addWidgetByArea(Area area, QWidget *w) {
  Q_D(InteractionLayout);
  if (d->m_widgets_map.contains(area) && !d->m_widgets_map[area].isEmpty()) {
    QWidget *oldw = d->m_widgets_map[area].at(0);
    if (qobject_cast<WidgetGroup *>(oldw)) {
      WidgetGroup *wg = qobject_cast<WidgetGroup *>(oldw);
      wg->addWidget(w);
    } else {
      d->m_widgets_map[area].clear();
      WidgetGroup *wg = new WidgetGroup(parentWidget());
      wg->addWidget(oldw);
      wg->addWidget(w);
      d->m_widgets_map[area].prepend(wg);
      wg->show();
      wg->raise();
    }

    return true;
  } else {
    QList<QWidget *> list;
    list.append(w);
    d->m_widgets_map.insert(area, list);
  }
}

bool InteractionLayout::setCornerBelongs(Corner corner, Area area) {
  Q_D(InteractionLayout);
  d->m_corner_belong.insert(corner, area);  // FIXME  corner 与 area 不相交
  return true;
}

// QLayoutItem *InteractionLayout::unplug(QWidget *widget, bool group) {
// return;
// }

int InteractionLayout::smartSpacing(QStyle::PixelMetric pm) const {
  QObject *parent = this->parent();
  if (!parent) {
    return -1;
  } else if (parent->isWidgetType()) {
    QWidget *pw = static_cast<QWidget *>(parent);
    return pw->style()->pixelMetric(pm, nullptr, pw);
  } else {
    return static_cast<QLayout *>(parent)->spacing();
  }
}

void InteractionLayout::setWidgetIntoArea(QWidget *widget,
                                          const QPoint &mousePos) {
  addWidgetByArea(LayoutInterface::Left_Area, widget);
}

QVarLengthArray<QRect, 4> InteractionLayout::getRectForArea(bool isHover) {
  Q_D(InteractionLayout);
  QVarLengthArray<QRect, 4> rectList = {QRect(), QRect(), QRect(), QRect()};
  int lMargin = 0, tMargin = 0, rMargin = 0, bMargin = 0;
  getContentsMargins(&lMargin, &tMargin, &rMargin, &bMargin);
  QRect rect = d->m_rect;
  rect.adjust(lMargin, tMargin, -rMargin, -bMargin);

  if (d->m_corner_belong[LayoutInterface::Left_Top_Corner] ==
          LayoutInterface::Left_Area &&
      d->m_corner_belong[LayoutInterface::Left_Bottom_Corner] ==
          LayoutInterface::Left_Area) {
    rectList[LayoutInterface::Left_Area] =
        QRect(lMargin, tMargin, 200, rect.height());
  } else if (d->m_corner_belong[LayoutInterface::Left_Top_Corner] ==
                 LayoutInterface::Left_Area ||
             d->m_widgets_map[LayoutInterface::Top_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Top_Area].isEmpty())
      rectList[LayoutInterface::Left_Area] =
          QRect(lMargin, tMargin, 200, rect.height());
    else
      rectList[LayoutInterface::Left_Area] =
          QRect(lMargin, tMargin, 200, rect.height() - 200 - verticalSpacing());
  } else if (d->m_corner_belong[LayoutInterface::Left_Bottom_Corner] ==
                 LayoutInterface::Left_Area ||
             d->m_widgets_map[LayoutInterface::Bottom_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Bottom_Area].isEmpty())
      rectList[LayoutInterface::Left_Area] =
          QRect(lMargin, tMargin, 200, rect.height());
    else
      rectList[LayoutInterface::Left_Area] =
          QRect(lMargin, tMargin + 200 + verticalSpacing(), 200,
                rect.height() - 200 - verticalSpacing());
  } else {
    rectList[LayoutInterface::Left_Area] =
        QRect(lMargin, tMargin + 200 + verticalSpacing(), 200,
              rect.height() - 400 - verticalSpacing() * 2);
  }

  if (d->m_corner_belong[LayoutInterface::Right_Top_Corner] ==
          LayoutInterface::Right_Area &&
      d->m_corner_belong[LayoutInterface::Right_Bottom_Corner] ==
          LayoutInterface::Right_Area) {
    rectList[LayoutInterface::Right_Area] =
        QRect(rect.right() - 200, tMargin, 200, rect.height());
  } else if (d->m_corner_belong[LayoutInterface::Right_Top_Corner] ==
                 LayoutInterface::Right_Area ||
             d->m_widgets_map[LayoutInterface::Top_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Top_Area].isEmpty())
      rectList[LayoutInterface::Right_Area] =
          QRect(rect.right() - 200, tMargin, 200, rect.height());
    else
      rectList[LayoutInterface::Right_Area] =
          QRect(rect.right() - 200, tMargin, 200,
                rect.height() - 200 - verticalSpacing());
  } else if (d->m_corner_belong[LayoutInterface::Right_Bottom_Corner] ==
                 LayoutInterface::Right_Area ||
             d->m_widgets_map[LayoutInterface::Bottom_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Bottom_Area].isEmpty())
      rectList[LayoutInterface::Right_Area] =
          QRect(rect.right() - 200, tMargin, 200, rect.height());
    else
      rectList[LayoutInterface::Right_Area] =
          QRect(rect.right() - 200, tMargin - 200 - verticalSpacing(), 200,
                rect.height() - 200 - verticalSpacing());
  } else {
    rectList[LayoutInterface::Right_Area] =
        QRect(rect.right() - 200, rect.top() + 200, 200,
              rect.height() - 400 - verticalSpacing() * 2);
  }

  if (d->m_corner_belong[LayoutInterface::Left_Top_Corner] ==
          LayoutInterface::Top_Area &&
      d->m_corner_belong[LayoutInterface::Right_Top_Corner] ==
          LayoutInterface::Top_Area) {
    rectList[LayoutInterface::Top_Area] =
        QRect(lMargin, tMargin, rect.width(), 200);
  } else if (d->m_corner_belong[LayoutInterface::Left_Top_Corner] ==
                 LayoutInterface::Top_Area ||
             d->m_widgets_map[LayoutInterface::Left_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Left_Area].isEmpty())
      rectList[LayoutInterface::Top_Area] =
          QRect(lMargin, tMargin, rect.width(), 200);
    else
      rectList[LayoutInterface::Top_Area] = QRect(
          lMargin, tMargin, rect.width() - 200 - horizontalSpacing(), 200);
  } else if (d->m_corner_belong[LayoutInterface::Right_Top_Corner] ==
                 LayoutInterface::Top_Area ||
             d->m_widgets_map[LayoutInterface::Right_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Right_Area].isEmpty())
      rectList[LayoutInterface::Top_Area] =
          QRect(lMargin, tMargin, rect.width(), 200);
    else
      rectList[LayoutInterface::Top_Area] =
          QRect(lMargin + 200 + horizontalSpacing(), tMargin,
                rect.width() - 200 - horizontalSpacing(), 200);
  } else {
    rectList[LayoutInterface::Top_Area] =
        QRect(lMargin + 200 + horizontalSpacing(), tMargin,
              rect.width() - 400 - horizontalSpacing() * 2, 200);
  }

  if (d->m_corner_belong[LayoutInterface::Left_Bottom_Corner] ==
          LayoutInterface::Bottom_Area &&
      d->m_corner_belong[LayoutInterface::Right_Bottom_Corner] ==
          LayoutInterface::Bottom_Area) {
    rectList[LayoutInterface::Bottom_Area] =
        QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
  } else if (d->m_corner_belong[LayoutInterface::Left_Bottom_Corner] ==
                 LayoutInterface::Bottom_Area ||
             d->m_widgets_map[LayoutInterface::Left_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Left_Area].isEmpty())
      rectList[LayoutInterface::Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
    else
      rectList[LayoutInterface::Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200,
                rect.width() - 200 - horizontalSpacing(), 200);
  } else if (d->m_corner_belong[LayoutInterface::Right_Bottom_Corner] ==
                 LayoutInterface::Bottom_Area ||
             d->m_widgets_map[LayoutInterface::Right_Area].isEmpty()) {
    if (isHover && d->m_widgets_map[LayoutInterface::Right_Area].isEmpty())
      rectList[LayoutInterface::Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
    else
      rectList[LayoutInterface::Bottom_Area] =
          QRect(rect.left() + 200 + horizontalSpacing(), rect.bottom() - 200,
                rect.width() - 200 - horizontalSpacing(), 200);
  } else {
    rectList[LayoutInterface::Bottom_Area] =
        QRect(lMargin + 200 + horizontalSpacing(), rect.bottom() - 200,
              rect.width() - 400 - horizontalSpacing() * 2, 200);
  }

  return rectList;
}

InteractionItem::InteractionItem(QWidget *widget) : QWidgetItemV2(widget) {}

InteractionItem::~InteractionItem() {}
