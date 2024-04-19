#ifndef INTERACTIONLAYOUT_H
#define INTERACTIONLAYOUT_H

#include <QLayout>
#include <QLayoutItem>
#include <QObject>
#include <QPointer>
#include <QRubberBand>
#include <QStyle>
#include <QVarLengthArray>

class InteractionLayoutPrivate;

/**
 *  框架中需要使用的布局通用接口
 *
 * @brief The LayoutInterface class
 */
class LayoutInterface {
 public:
  enum Area { Left_Area = 0, Right_Area, Top_Area, Bottom_Area, UnDefine_Area };
  enum Corner {
    Left_Top_Corner = 0,
    Right_Top_Corner,
    Left_Bottom_Corner,
    Right_Bottom_Corner
  };

  /**
   *    将widget添加到指定的边缘区域
   *
   * @brief addWidgetByArea
   * @param area
   * @param w
   * @return
   */
  virtual bool addWidgetByArea(Area area, QWidget *w) = 0;

  /**
   *    设置布局中四个角的归属区域
   *
   * @brief setCornerBelongs
   * @param corner
   * @param area
   */
  virtual bool setCornerBelongs(Corner corner, Area area) = 0;
};

class InteractionLayout : public QLayout, public LayoutInterface {
  Q_OBJECT
  InteractionLayoutPrivate *d;

 public:
  InteractionLayout(QWidget *parent);
  InteractionLayout();
  ~InteractionLayout();

  void addItem(QLayoutItem *) override;

  Qt::Orientations expandingDirections() const override;
  QSize minimumSize() const override;
  QSize maximumSize() const override;
  QSize sizeHint() const override;
  void setGeometry(const QRect &) override;
  QLayoutItem *itemAt(int index) const;
  QLayoutItem *takeAt(int index);
  int count() const override;

  bool hasHeightForWidth() const;
  int heightForWidth(int) const;
  int minimumHeightForWidth(int) const;

  int horizontalSpacing() const;
  int verticalSpacing() const;

  static QSize closestAcceptableSize(const QWidget *w, const QSize &s);

  QLayoutItem *unplug(QWidget *widget);

  bool plug(QWidget *widget, const QPoint &mousePos);

  void hover(QWidget *widget, const QPoint &mousePos, bool isMoving = true);

  bool addWidgetByArea(Area area, QWidget *w);

  bool setCornerBelongs(Corner corner, Area area);

  void setGapIndicatorHide();

 private:
  int smartSpacing(QStyle::PixelMetric pm) const;

  void setWidgetIntoArea(QWidget *widget, const QPoint &mousePos);

  /**
   *    计算每个吸附区域的Rect
   *
   * @brief getRectForArea
   * @param isHover
   * 为false,是计算布局应用的区域;为true，计算外部区域hover时应用的区域
   * @return
   */
  QVarLengthArray<QRect, 4> getRectForArea(Area hover_area);
};

class InteractionItem : public QWidgetItemV2 {
 public:
  explicit InteractionItem(QWidget *widget);
  ~InteractionItem();
};

#endif  // INTERACTIONLAYOUT_H
