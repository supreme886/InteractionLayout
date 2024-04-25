#ifndef GLOBAL_H
#define GLOBAL_H

#include <QApplication>'
#include <QPointer>
#include <QStyle>

class BaseSubWidget;

enum DragState {
  DraggingInactive,
  DraggingMousePressed,
  DraggingTab,
  DraggingFloatingWidget
};

enum MousePosition {
  Left = 0,
  Right,
  Top,
  Bottom,
  TopLeft,
  BottomRight,
  BottomLeft,
  TopRight,
  Center,
  Nowhere
};

enum Area { Left_Area = 0, Right_Area, Top_Area, Bottom_Area, UnDefine_Area };

enum Corner {
  Left_Top_Corner = 0,
  Right_Top_Corner,
  Left_Bottom_Corner,
  Right_Bottom_Corner
};

struct TabInfoStruct {
  TabInfoStruct(QPointer<BaseSubWidget> w)
      : m_tagWidget(w),
        m_Icon(qApp->style()->standardIcon(QStyle::SP_TitleBarCloseButton)) {}
  QPointer<BaseSubWidget> m_tagWidget;
  QIcon m_Icon;
  QString m_titleName{"Null"};
  bool m_canSpliting{true};
};

#endif  // GLOBAL_H
