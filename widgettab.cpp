#include "widgettab.h"

#include <QBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QToolButton>

#include "global.h"

struct WidgetTabPrivate {
  WidgetTabPrivate(QWidget *tagWidget) : m_tagWidget(tagWidget) {}

  void saveDragStartMousePosition(const QPoint &GlobalPos);

  bool isDraggingState(DragState dragState);

  QWidget *m_tagWidget{nullptr};
  QPoint GlobalDragStartMousePosition;
  QPoint DragStartMousePosition;
  DragState m_DragState;
  QIcon m_Icon;
  QString m_titleName;
  QLabel *m_titleLabel;
};

void WidgetTabPrivate::saveDragStartMousePosition(const QPoint &GlobalPos) {
  GlobalDragStartMousePosition = GlobalPos;
  DragStartMousePosition = m_tagWidget->mapFromGlobal(GlobalPos);
}

bool WidgetTabPrivate::isDraggingState(DragState dragState) {
  return m_DragState == dragState;
}

WidgetTab::WidgetTab(QWidget *tagWidget, QWidget *parent)
    : Super(parent), d(new WidgetTabPrivate(tagWidget)) {}

void WidgetTab::setIcon(const QIcon &icon) { d->m_Icon = icon; }

QIcon WidgetTab::getIcon() { return d->m_Icon; }

void WidgetTab::setTitleName(const QString &title) { d->m_titleName = title; }

QString WidgetTab::titleName() { return d->m_titleName; }

void WidgetTab::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    e->accept();
    d->saveDragStartMousePosition(e->globalPos());
    d->m_DragState = DraggingMousePressed;
    Q_EMIT clicked();
    return;
  }
  Super::mousePressEvent(e);
}

void WidgetTab::mouseReleaseEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    auto CurrentDragState = d->m_DragState;
    d->GlobalDragStartMousePosition = QPoint();
    d->DragStartMousePosition = QPoint();
    d->m_DragState = DraggingInactive;

    switch (CurrentDragState) {
      case DraggingTab:
        // End of tab moving, emit signal
        // if (d->DockArea) {
        //   e->accept();
        //   Q_EMIT moved(e->globalPos());
        // }
        break;

      case DraggingFloatingWidget:
        e->accept();
        break;

      default:
        break;
    }
  }

  Super::mouseReleaseEvent(e);
}

void WidgetTab::mouseMoveEvent(QMouseEvent *e) {
  if (!(e->buttons() & Qt::LeftButton) ||
      d->isDraggingState(DraggingInactive)) {
    d->m_DragState = DraggingInactive;
    Super::mouseMoveEvent(e);
    return;
  }

  Super::mouseMoveEvent(e);
}

void WidgetTab::contextMenuEvent(QContextMenuEvent *e) {}
