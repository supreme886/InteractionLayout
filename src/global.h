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

struct TabInfoStruct {
  TabInfoStruct(QPointer<BaseSubWidget> w)
      : m_tagWidget(w),
        m_Icon(qApp->style()->standardIcon(QStyle::SP_TitleBarCloseButton)) {}
  QPointer<BaseSubWidget> m_tagWidget;
  QIcon m_Icon;
  QString m_titleName{"Null"};
};

#endif  // GLOBAL_H
