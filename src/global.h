#ifndef GLOBAL_H
#define GLOBAL_H

#include <QApplication>
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
  Left = 0x00000000,
  Right = 0x00000001,
  Top = 0x00000002,
  Bottom = 0x00000004,
  TopLeft = 0x00000008,
  BottomRight = 0x00000010,
  BottomLeft = 0x00000012,
  TopRight = 0x00000020,
  Center = 0x00000040,
  Nowhere = 0x000000ff
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
      : m_tag_widget(w),
        m_Icon(qApp->style()->standardIcon(QStyle::SP_TitleBarCloseButton)) {}
  QPointer<BaseSubWidget> m_tag_widget;
  QIcon m_Icon;
  QString m_title_name{"Null"};
  bool m_can_spliting{true};
  bool m_can_closed{true};
};

#endif  // GLOBAL_H
