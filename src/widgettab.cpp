#include "widgettab.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>

#include "basewidget.h"
#include "global.h"

struct WidgetTabPrivate {
  WidgetTabPrivate(IWidgetTab *_pub, TabInfoStruct *info);
  ~WidgetTabPrivate() {}

  void saveDragStartMousePosition(const QPoint &GlobalPos);

  bool isDraggingState(DragState dragState);

  IWidgetTab *_this{nullptr};
  QPoint GlobalDragStartMousePosition;
  QPoint DragStartMousePosition;
  DragState m_DragState;
  TabInfoStruct *m_tabInfo;
};

WidgetTabPrivate::WidgetTabPrivate(IWidgetTab *_pub, TabInfoStruct *info)
    : _this(_pub), m_tabInfo(info) {}

void WidgetTabPrivate::saveDragStartMousePosition(const QPoint &GlobalPos) {
  GlobalDragStartMousePosition = GlobalPos;
  DragStartMousePosition = _this->mapFromGlobal(GlobalPos);
}

bool WidgetTabPrivate::isDraggingState(DragState dragState) {
  return m_DragState == dragState;
}

IWidgetTab::IWidgetTab(TabInfoStruct *tab, QWidget *parent)
    : Super(parent), d(new WidgetTabPrivate(this, tab)) {
  setAttribute(Qt::WA_DeleteOnClose);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setCheckable(true);
}

IWidgetTab::~IWidgetTab() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

QIcon IWidgetTab::getIcon() { return d->m_tabInfo->m_Icon; }

QString IWidgetTab::titleName() { return d->m_tabInfo->m_titleName; }

void IWidgetTab::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    e->accept();
    d->saveDragStartMousePosition(e->globalPos());
    d->m_DragState = DraggingMousePressed;
    setChecked(!isChecked());
    Q_EMIT clicked();
    return;
  }
  Super::mousePressEvent(e);
}

void IWidgetTab::mouseReleaseEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    auto CurrentDragState = d->m_DragState;
    d->GlobalDragStartMousePosition = QPoint();
    d->DragStartMousePosition = QPoint();
    d->m_DragState = DraggingInactive;

    switch (CurrentDragState) {
      case DraggingTab:
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

void IWidgetTab::mouseMoveEvent(QMouseEvent *e) {
  if (!(e->buttons() & Qt::LeftButton) ||
      d->isDraggingState(DraggingInactive)) {
    d->m_DragState = DraggingInactive;
    Super::mouseMoveEvent(e);
    return;
  }

  QPoint point = e->pos() - d->DragStartMousePosition;
  if (d->isDraggingState(DraggingMousePressed) &&
      point.manhattanLength() > 10) {
    if (d->m_tabInfo->m_tagWidget) {
      d->m_tabInfo->m_tagWidget->startSplits();
      d->m_DragState = DraggingTab;
      Q_EMIT tabsplit();
    }
  }

  Super::mouseMoveEvent(e);
}

void IWidgetTab::contextMenuEvent(QContextMenuEvent *e) {}

WidgetTab::WidgetTab(TabInfoStruct *tab, QWidget *parent) : Super(tab, parent) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(15, 0, 15, 0);
  layout->setSpacing(5);
  layout->addWidget(new QLabel(titleName()));

  QPushButton *close_btn = new QPushButton;
  close_btn->setFixedSize(20, 20);
  close_btn->setIcon(getIcon());
  // close_btn->setIconSize(QSize(8, 8));
  connect(close_btn, &QPushButton::clicked, this, &WidgetTab::closed);
  layout->addWidget(close_btn);
  setChecked(true);
}

WidgetTab::~WidgetTab() {}

void WidgetTab::paintEvent(QPaintEvent *e) {
  Q_UNUSED(e);
  QPainter painter(this);
  painter.setPen(Qt::NoPen);
  painter.fillRect(rect(),
                   QBrush(isChecked() ? QColor("#FFFFFF") : QColor("#E0E5ED")));
}
