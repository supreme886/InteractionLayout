#include "interactionlayout.h"

#include <QDebug>
#include <QVarLengthArray>
#include <QWidget>

#include "dragmanager.h"
#include "widgetgroup.h"

class InteractionLayoutPrivate {
 public:
  InteractionLayoutPrivate() {
    m_corner_belong.insert(Left_Top_Corner, Top_Area);
    m_corner_belong.insert(Right_Top_Corner, Right_Area);
    m_corner_belong.insert(Right_Bottom_Corner, Bottom_Area);
    m_corner_belong.insert(Left_Bottom_Corner, Left_Area);
  }

  void setCenterWidget(QLayoutItem *) const;
  QLayoutItem *getCenterWidget() const;

  mutable QLayoutItem *m_center_wIdget{nullptr};
  QMap<Area, QList<QWidgetItemV2 *>> m_widgets_map;
  QMap<Corner, Area> m_corner_belong;
  int m_hSpace{5};
  int m_vSpace{5};
  QRect m_rect;
  Area m_hover{UnDefine_Area};
  QPointer<QRubberBand> gapIndicator;
  bool m_first_flag{true};
};

void InteractionLayoutPrivate::setCenterWidget(QLayoutItem *item) const {
  if (item == m_center_wIdget) return;
  if (m_center_wIdget) {
    delete m_center_wIdget;
  }
  m_center_wIdget = item;
}

QLayoutItem *InteractionLayoutPrivate::getCenterWidget() const {
  return m_center_wIdget;
}

InteractionLayout::InteractionLayout(QWidget *parent)
    : QLayout(parent), d(new InteractionLayoutPrivate) {
  DragManager::instance()->addDragFeaturesWidget(parent);
}

InteractionLayout::InteractionLayout()
    : QLayout(0), d(new InteractionLayoutPrivate) {}

InteractionLayout::~InteractionLayout() {}

void InteractionLayout::addItem(QLayoutItem *item) { d->setCenterWidget(item); }

Qt::Orientations InteractionLayout::expandingDirections() const { return 0; }

QSize InteractionLayout::minimumSize() const { return QSize(); }

QSize InteractionLayout::maximumSize() const { return QSize(); }

QSize InteractionLayout::sizeHint() const { return QSize(); }

void InteractionLayout::setGeometry(const QRect &r) {
  d->m_rect = r;
  if (d->m_center_wIdget) {
    d->m_center_wIdget->widget()->lower();
    d->m_center_wIdget->setGeometry(r);
  }
  QVarLengthArray<QRect, 4> rectList = getRectForArea(d->m_hover);
  for (auto iter = d->m_widgets_map.begin(); iter != d->m_widgets_map.end();
       iter++) {
    if (iter.value().size() && iter.key() < rectList.size()) {
      iter.value().at(0)->setGeometry(rectList.at(iter.key()));
      qDebug() << Q_FUNC_INFO << iter.value().at(0)->widget()->size();
    }
  }
}

QLayoutItem *InteractionLayout::itemAt(int index) const { return nullptr; }

QLayoutItem *InteractionLayout::takeAt(int index) { return nullptr; }

int InteractionLayout::count() const { return 1; }

bool InteractionLayout::hasHeightForWidth() const { return false; }

int InteractionLayout::heightForWidth(int i) const { return -1; }

int InteractionLayout::minimumHeightForWidth(int w) const {
  return heightForWidth(w);
}

int InteractionLayout::horizontalSpacing() const {
  if (d->m_hSpace >= 0) {
    return d->m_hSpace;
  } else {
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
  }
}

int InteractionLayout::verticalSpacing() const {
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
  for (auto iter = d->m_widgets_map.begin(); iter != d->m_widgets_map.end();
       iter++) {
    if (iter.value().size()) {
      for (int i = 0; i < iter.value().size(); i++) {
        if (iter.value().at(i)->widget() == widget) {
          iter.value().takeAt(i);
          return nullptr;
        }
      }
    }
  }
  return nullptr;
}

bool InteractionLayout::plug(QWidget *widget, const QPoint &mousePos) {
  bool plug_success = false;
  QVarLengthArray<QRect, 4> rectList = getRectForArea(d->m_hover);
  int targetArea = -1;
  for (int i = 0; i < 4; i++) {
    if (rectList.at(i).contains(parentWidget()->mapFromGlobal(mousePos))) {
      targetArea = i;
    }
  }

  if (targetArea >= 0 && targetArea < 4) {
    widget->setParent(parentWidget());
    widget->show();
    addWidgetByArea((Area)targetArea, widget);
    plug_success = true;
  } /*else {
    if (d->gapIndicator) {
      delete d->gapIndicator;
    }
  }*/

  if (d->gapIndicator) {
    delete d->gapIndicator;
  }

  update();
  return plug_success;
}

void InteractionLayout::hover(QWidget *widget, const QPoint &mousePos,
                              bool isMoving) {
  QVarLengthArray<QRect, 4> rectList = getRectForHoverArea(d->m_hover);
  int targetArea = -1;
  for (int i = 0; i < 4; i++) {
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
}

bool InteractionLayout::addWidgetByArea(Area area, QWidget *w) {
  if (d->m_widgets_map.contains(area) && !d->m_widgets_map[area].isEmpty()) {
    QWidget *oldw = d->m_widgets_map[area].at(0)->widget();
    if (qobject_cast<WidgetGroup *>(oldw)) {
      WidgetGroup *wg = qobject_cast<WidgetGroup *>(oldw);
      wg->setResizeHandle(true);
      wg->setProperty("Area", area);
      wg->addWidget(w);
    } else {
      d->m_widgets_map[area].clear();
      WidgetGroup *wg = new WidgetGroup(parentWidget());
      wg->setResizeHandle(true);
      wg->setProperty("Area", area);
      wg->addWidget(oldw);
      wg->addWidget(w);
      d->m_widgets_map[area].prepend(new QWidgetItemV2(wg));
      wg->show();
      wg->raise();
    }
    return true;
  } else {
    QList<QWidgetItemV2 *> list;
    WidgetGroup *wg = new WidgetGroup(parentWidget());
    wg->setResizeHandle(true);
    wg->setProperty("Area", area);
    wg->addWidget(w);
    wg->show();
    wg->raise();
    list.append(new QWidgetItemV2(wg));
    d->m_widgets_map.insert(area, list);
  }
}

bool InteractionLayout::setCornerBelongs(Corner corner, Area area) {
  d->m_corner_belong.insert(corner, area);  // FIXME  corner 与 area 不相交
  return true;
}

void InteractionLayout::setGapIndicatorHide() {
  if (d->gapIndicator) delete d->gapIndicator;
}

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
  addWidgetByArea(Left_Area, widget);
}

QVarLengthArray<QRect, 4> InteractionLayout::getRectForArea(Area hover_area) {
  qDebug() << Q_FUNC_INFO << hover_area;
  QVarLengthArray<QRect, 4> rectList = {QRect(), QRect(), QRect(), QRect()};
  int lMargin = 0, tMargin = 0, rMargin = 0, bMargin = 0;
  getContentsMargins(&lMargin, &tMargin, &rMargin, &bMargin);
  QRect rect = d->m_rect;
  rect.adjust(lMargin, tMargin, -rMargin, -bMargin);

  // QSize top = d->m_widgets_map[Top_Area].size()
  //                 ? d->m_widgets_map[Top_Area].at(0)->sizeHint()
  //                 : QSize(0, 0);
  // QSize left = d->m_widgets_map[Left_Area].size()
  //                  ? d->m_widgets_map[Left_Area].at(0)->sizeHint()
  //                  : QSize(0, 0);
  // QSize right = d->m_widgets_map[Right_Area].size()
  //                   ? d->m_widgets_map[Right_Area].at(0)->sizeHint()
  //                   : QSize(0, 0);
  // QSize bottom = d->m_widgets_map[Bottom_Area].size()
  //                    ? d->m_widgets_map[Bottom_Area].at(0)->sizeHint()
  //                    : QSize(0, 0);

  // QRect center = rect.adjusted(left.width(), top.height(), -right.width(),
  //                              -bottom.height());
  // qDebug() << Q_FUNC_INFO << center;

  // QRect top_rect = QRect(rect.left(), rect.top(), rect.width(),
  // top.height()); QRect left_rect =
  //     QRect(rect.left(), center.top(), left.width(), center.height());
  // QRect right_rect =
  //     QRect(center.right(), center.top(), right.width(), center.height());
  // QRect bottom_rect =
  //     QRect(rect.left(), center.bottom(), rect.width(), bottom.height());

  // rectList[Left_Area] = left_rect;
  // rectList[Top_Area] = top_rect;
  // rectList[Right_Area] = right_rect;
  // rectList[Bottom_Area] = bottom_rect;
  // return rectList;

  if (d->m_corner_belong[Left_Top_Corner] == Left_Area &&
      d->m_corner_belong[Left_Bottom_Corner] == Left_Area) {
    rectList[Left_Area] = QRect(lMargin, tMargin, 200, rect.height());
  } else if (d->m_corner_belong[Left_Top_Corner] == Left_Area) {
    if (d->m_widgets_map[Bottom_Area].isEmpty())
      rectList[Left_Area] = QRect(lMargin, tMargin, 200, rect.height());
    else
      rectList[Left_Area] =
          QRect(lMargin, tMargin, 200, rect.height() - 200 - verticalSpacing());
  } else if (d->m_corner_belong[Left_Bottom_Corner] == Left_Area) {
    if (d->m_widgets_map[Top_Area].isEmpty())
      rectList[Left_Area] = QRect(lMargin, tMargin, 200, rect.height());
    else
      rectList[Left_Area] = QRect(lMargin, tMargin + 200 + verticalSpacing(),
                                  200, rect.height() - 200 - verticalSpacing());
  } else {
    rectList[Left_Area] = QRect(lMargin, tMargin + 200 + verticalSpacing(), 200,
                                rect.height() - 400 - verticalSpacing() * 2);
  }

  if (d->m_corner_belong[Right_Top_Corner] == Right_Area &&
      d->m_corner_belong[Right_Bottom_Corner] == Right_Area) {
    rectList[Right_Area] =
        QRect(rect.right() - 200, tMargin, 200, rect.height());
  } else if (d->m_corner_belong[Right_Top_Corner] == Right_Area) {
    if (d->m_widgets_map[Top_Area].isEmpty())
      rectList[Right_Area] =
          QRect(rect.right() - 200, tMargin, 200, rect.height());
    else
      rectList[Right_Area] = QRect(rect.right() - 200, tMargin, 200,
                                   rect.height() - 200 - verticalSpacing());
  } else if (d->m_corner_belong[Right_Bottom_Corner] == Right_Area) {
    if (d->m_widgets_map[Bottom_Area].isEmpty())
      rectList[Right_Area] =
          QRect(rect.right() - 200, tMargin, 200, rect.height());
    else
      rectList[Right_Area] =
          QRect(rect.right() - 200, tMargin - 200 - verticalSpacing(), 200,
                rect.height() - 200 - verticalSpacing());
  } else {
    rectList[Right_Area] = QRect(rect.right() - 200, rect.top() + 200, 200,
                                 rect.height() - 400 - verticalSpacing() * 2);
  }

  if (d->m_corner_belong[Left_Top_Corner] == Top_Area &&
      d->m_corner_belong[Right_Top_Corner] == Top_Area) {
    rectList[Top_Area] = QRect(lMargin, tMargin, rect.width(), 200);
  } else if (d->m_corner_belong[Left_Top_Corner] == Top_Area) {
    if (d->m_widgets_map[Left_Area].isEmpty())
      rectList[Top_Area] = QRect(lMargin, tMargin, rect.width(), 200);
    else
      rectList[Top_Area] = QRect(lMargin, tMargin,
                                 rect.width() - 200 - horizontalSpacing(), 200);
  } else if (d->m_corner_belong[Right_Top_Corner] == Top_Area) {
    if (d->m_widgets_map[Right_Area].isEmpty())
      rectList[Top_Area] = QRect(lMargin, tMargin, rect.width(), 200);
    else
      rectList[Top_Area] = QRect(lMargin + 200 + horizontalSpacing(), tMargin,
                                 rect.width() - 200 - horizontalSpacing(), 200);
  } else {
    rectList[Top_Area] =
        QRect(lMargin + 200 + horizontalSpacing(), tMargin,
              rect.width() - 400 - horizontalSpacing() * 2, 200);
  }

  if (d->m_corner_belong[Left_Bottom_Corner] == Bottom_Area &&
      d->m_corner_belong[Right_Bottom_Corner] == Bottom_Area) {
    rectList[Bottom_Area] =
        QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
  } else if (d->m_corner_belong[Left_Bottom_Corner] == Bottom_Area) {
    if (d->m_widgets_map[Right_Area].isEmpty())
      rectList[Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
    else
      rectList[Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200,
                rect.width() - 200 - horizontalSpacing(), 200);
  } else if (d->m_corner_belong[Right_Bottom_Corner] == Bottom_Area) {
    if (d->m_widgets_map[Left_Area].isEmpty())
      rectList[Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
    else
      rectList[Bottom_Area] =
          QRect(rect.left() + 200 + horizontalSpacing(), rect.bottom() - 200,
                rect.width() - 200 - horizontalSpacing(), 200);
  } else {
    rectList[Bottom_Area] =
        QRect(lMargin + 200 + horizontalSpacing(), rect.bottom() - 200,
              rect.width() - 400 - horizontalSpacing() * 2, 200);
  }

  return rectList;
}

QVarLengthArray<QRect, 4> InteractionLayout::getRectForHoverArea(
    Area hover_area) {
  qDebug() << Q_FUNC_INFO << hover_area;
  QVarLengthArray<QRect, 4> rectList = {QRect(), QRect(), QRect(), QRect()};
  int lMargin = 0, tMargin = 0, rMargin = 0, bMargin = 0;
  getContentsMargins(&lMargin, &tMargin, &rMargin, &bMargin);
  QRect rect = d->m_rect;
  rect.adjust(lMargin, tMargin, -rMargin, -bMargin);

  // QSize top = d->m_widgets_map[Top_Area].size()
  //                 ? d->m_widgets_map[Top_Area].at(0)->sizeHint()
  //                 : QSize(0, 0);
  // QSize left = d->m_widgets_map[Left_Area].size()
  //                  ? d->m_widgets_map[Left_Area].at(0)->sizeHint()
  //                  : QSize(0, 0);
  // QSize right = d->m_widgets_map[Right_Area].size()
  //                   ? d->m_widgets_map[Right_Area].at(0)->sizeHint()
  //                   : QSize(0, 0);
  // QSize bottom = d->m_widgets_map[Bottom_Area].size()
  //                    ? d->m_widgets_map[Bottom_Area].at(0)->sizeHint()
  //                    : QSize(0, 0);

  // QRect center = rect.adjusted(left.width(), top.height(), -right.width(),
  //                              -bottom.height());
  // qDebug() << Q_FUNC_INFO << center;

  // QRect top_rect = QRect(rect.left(), rect.top(), rect.width(),
  // top.height()); QRect left_rect =
  //     QRect(rect.left(), center.top(), left.width(), center.height());
  // QRect right_rect =
  //     QRect(center.right(), center.top(), right.width(), center.height());
  // QRect bottom_rect =
  //     QRect(rect.left(), center.bottom(), rect.width(), bottom.height());

  // rectList[Left_Area] = left_rect;
  // rectList[Top_Area] = top_rect;
  // rectList[Right_Area] = right_rect;
  // rectList[Bottom_Area] = bottom_rect;
  // return rectList;

  if (d->m_corner_belong[Left_Top_Corner] == Left_Area &&
      d->m_corner_belong[Left_Bottom_Corner] == Left_Area) {
    rectList[Left_Area] = QRect(lMargin, tMargin, 200, rect.height());
  } else if (d->m_corner_belong[Left_Top_Corner] == Left_Area) {
    if (d->m_widgets_map[Bottom_Area].isEmpty())
      rectList[Left_Area] = QRect(lMargin, tMargin, 200, rect.height());
    else
      rectList[Left_Area] =
          QRect(lMargin, tMargin, 200, rect.height() - 200 - verticalSpacing());
  } else if (d->m_corner_belong[Left_Bottom_Corner] == Left_Area) {
    if (d->m_widgets_map[Top_Area].isEmpty())
      rectList[Left_Area] = QRect(lMargin, tMargin, 200, rect.height());
    else
      rectList[Left_Area] = QRect(lMargin, tMargin + 200 + verticalSpacing(),
                                  200, rect.height() - 200 - verticalSpacing());
  } else {
    rectList[Left_Area] = QRect(lMargin, tMargin + 200 + verticalSpacing(), 200,
                                rect.height() - 400 - verticalSpacing() * 2);
  }

  if (d->m_corner_belong[Right_Top_Corner] == Right_Area &&
      d->m_corner_belong[Right_Bottom_Corner] == Right_Area) {
    rectList[Right_Area] =
        QRect(rect.right() - 200, tMargin, 200, rect.height());
  } else if (d->m_corner_belong[Right_Top_Corner] == Right_Area) {
    if (d->m_widgets_map[Top_Area].isEmpty())
      rectList[Right_Area] =
          QRect(rect.right() - 200, tMargin, 200, rect.height());
    else
      rectList[Right_Area] = QRect(rect.right() - 200, tMargin, 200,
                                   rect.height() - 200 - verticalSpacing());
  } else if (d->m_corner_belong[Right_Bottom_Corner] == Right_Area) {
    if (d->m_widgets_map[Bottom_Area].isEmpty())
      rectList[Right_Area] =
          QRect(rect.right() - 200, tMargin, 200, rect.height());
    else
      rectList[Right_Area] =
          QRect(rect.right() - 200, tMargin - 200 - verticalSpacing(), 200,
                rect.height() - 200 - verticalSpacing());
  } else {
    rectList[Right_Area] = QRect(rect.right() - 200, rect.top() + 200, 200,
                                 rect.height() - 400 - verticalSpacing() * 2);
  }

  if (d->m_corner_belong[Left_Top_Corner] == Top_Area &&
      d->m_corner_belong[Right_Top_Corner] == Top_Area) {
    rectList[Top_Area] = QRect(lMargin, tMargin, rect.width(), 200);
  } else if (d->m_corner_belong[Left_Top_Corner] == Top_Area) {
    if (d->m_widgets_map[Left_Area].isEmpty())
      rectList[Top_Area] = QRect(lMargin, tMargin, rect.width(), 200);
    else
      rectList[Top_Area] = QRect(lMargin, tMargin,
                                 rect.width() - 200 - horizontalSpacing(), 200);
  } else if (d->m_corner_belong[Right_Top_Corner] == Top_Area) {
    if (d->m_widgets_map[Right_Area].isEmpty())
      rectList[Top_Area] = QRect(lMargin, tMargin, rect.width(), 200);
    else
      rectList[Top_Area] = QRect(lMargin + 200 + horizontalSpacing(), tMargin,
                                 rect.width() - 200 - horizontalSpacing(), 200);
  } else {
    rectList[Top_Area] =
        QRect(lMargin + 200 + horizontalSpacing(), tMargin,
              rect.width() - 400 - horizontalSpacing() * 2, 200);
  }

  if (d->m_corner_belong[Left_Bottom_Corner] == Bottom_Area &&
      d->m_corner_belong[Right_Bottom_Corner] == Bottom_Area) {
    rectList[Bottom_Area] =
        QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
  } else if (d->m_corner_belong[Left_Bottom_Corner] == Bottom_Area) {
    if (d->m_widgets_map[Right_Area].isEmpty())
      rectList[Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
    else
      rectList[Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200,
                rect.width() - 200 - horizontalSpacing(), 200);
  } else if (d->m_corner_belong[Right_Bottom_Corner] == Bottom_Area) {
    if (d->m_widgets_map[Left_Area].isEmpty())
      rectList[Bottom_Area] =
          QRect(lMargin, rect.bottom() - 200, rect.width(), 200);
    else
      rectList[Bottom_Area] =
          QRect(rect.left() + 200 + horizontalSpacing(), rect.bottom() - 200,
                rect.width() - 200 - horizontalSpacing(), 200);
  } else {
    rectList[Bottom_Area] =
        QRect(lMargin + 200 + horizontalSpacing(), rect.bottom() - 200,
              rect.width() - 400 - horizontalSpacing() * 2, 200);
  }

  return rectList;
}

InteractionItem::InteractionItem(QWidget *widget) : QWidgetItemV2(widget) {}

InteractionItem::~InteractionItem() {}
