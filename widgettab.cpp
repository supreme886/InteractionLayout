#include "widgettab.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QStyle>
#include <QToolButton>

#include "global.h"
#include "iwidget.h"

struct WidgetTabPrivate {
  WidgetTabPrivate(WidgetTab *_pub, IWidget *tagWidget);

  void saveDragStartMousePosition(const QPoint &GlobalPos);

  bool isDraggingState(DragState dragState);

  WidgetTab *_this{nullptr};
  IWidget *m_tagWidget{nullptr};
  QPoint GlobalDragStartMousePosition;
  QPoint DragStartMousePosition;
  DragState m_DragState;
  QIcon m_Icon;
  QString m_titleName{"Null"};
  QLabel *m_titleLabel{nullptr};
  QToolButton *m_closeBtn{nullptr};
};

WidgetTabPrivate::WidgetTabPrivate(WidgetTab *_pub, IWidget *tagWidget)
    : _this(_pub),
      m_tagWidget(tagWidget),
      m_Icon(qApp->style()->standardIcon(QStyle::SP_TitleBarCloseButton)),
      m_titleLabel(new QLabel(_this)),
      m_closeBtn(new QToolButton(_this)) {
  m_titleLabel->setText(m_titleName);
  m_closeBtn->setIcon(m_Icon);
  m_closeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);

  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, _this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(5);

  layout->addWidget(m_titleLabel);
  layout->addWidget(m_closeBtn);
}

void WidgetTabPrivate::saveDragStartMousePosition(const QPoint &GlobalPos) {
  GlobalDragStartMousePosition = GlobalPos;
  DragStartMousePosition = _this->mapFromGlobal(GlobalPos);
}

bool WidgetTabPrivate::isDraggingState(DragState dragState) {
  return m_DragState == dragState;
}

WidgetTab::WidgetTab(IWidget *tagWidget, QWidget *parent)
    : Super(parent), d(new WidgetTabPrivate(this, tagWidget)) {
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(d->m_closeBtn, &QToolButton::clicked, this, &WidgetTab::closed);
}

WidgetTab::~WidgetTab() { qDebug() << Q_FUNC_INFO; }

void WidgetTab::setIcon(const QIcon &icon) {
  d->m_Icon = icon;
  d->m_closeBtn->setIcon(d->m_Icon);
}

QIcon WidgetTab::getIcon() { return d->m_Icon; }

void WidgetTab::setTitleName(const QString &title) {
  d->m_titleName = title;
  d->m_titleLabel->setText(d->m_titleName);
}

QString WidgetTab::titleName() { return d->m_titleName; }

IWidget *WidgetTab::getIWidget() { return d->m_tagWidget; }

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

  QPoint point = e->pos() - d->DragStartMousePosition;
  if (d->isDraggingState(DraggingMousePressed) &&
      point.manhattanLength() > 10) {
    if (d->m_tagWidget) {
      QRect r = d->m_tagWidget->geometry();
      r.moveTopLeft(d->m_tagWidget->mapToGlobal(QPoint(0, 30)));
      qDebug() << Q_FUNC_INFO << r;
      d->m_tagWidget->setFloating(true, r);
      d->m_tagWidget->move(d->m_tagWidget->mapToGlobal(QPoint(0, 30)));

      d->m_DragState = DraggingTab;
      Q_EMIT tabsplit();
    }
  }

  Super::mouseMoveEvent(e);
}

void WidgetTab::contextMenuEvent(QContextMenuEvent *e) {}
