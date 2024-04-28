#include "widgettab.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>

#include "basesubwidget.h"
#include "global.h"

struct WidgetTabPrivate {
  WidgetTabPrivate(IWidgetTab *_pub, TabInfoStruct *info);
  ~WidgetTabPrivate() {}

  void savem_drag_start_mouse_position(const QPoint &global_pos);

  bool isDraggingState(DragState dragState);

  void setCanSpliting(bool canSplitig);

  bool isCanSpliting();

  IWidgetTab *m_this{nullptr};
  QPoint m_global_drag_start_mouse_position;
  QPoint m_drag_start_mouse_position;
  DragState m_drag_state;
  TabInfoStruct *m_tab_info;
  bool m_can_spliting{true};
};

WidgetTabPrivate::WidgetTabPrivate(IWidgetTab *_pub, TabInfoStruct *info)
    : m_this(_pub), m_tab_info(info) {}

void WidgetTabPrivate::savem_drag_start_mouse_position(
    const QPoint &global_pos) {
  m_global_drag_start_mouse_position = global_pos;
  m_drag_start_mouse_position = m_this->mapFromGlobal(global_pos);
}

bool WidgetTabPrivate::isDraggingState(DragState drag_state) {
  return m_drag_state == drag_state;
}

void WidgetTabPrivate::setCanSpliting(bool can_splitig) {
  m_can_spliting = can_splitig;
}

bool WidgetTabPrivate::isCanSpliting() { return m_can_spliting; }

IWidgetTab::IWidgetTab(TabInfoStruct *tab, QWidget *parent)
    : Super(parent), d(new WidgetTabPrivate(this, tab)) {
  setAttribute(Qt::WA_DeleteOnClose);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setCheckable(true);
  setMouseTracking(true);
}

IWidgetTab::~IWidgetTab() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

QIcon IWidgetTab::getIcon() { return d->m_tab_info->m_Icon; }

QString IWidgetTab::titleName() { return d->m_tab_info->m_titleName; }

void IWidgetTab::setCanspliting(bool can_spliting) {
  d->setCanSpliting(can_spliting);
}

bool IWidgetTab::isCanSpliting() {
  return d->isCanSpliting() && cursor().shape() == Qt::ArrowCursor;
}

void IWidgetTab::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    if (isCanSpliting()) {
      e->accept();
      d->savem_drag_start_mouse_position(e->globalPos());
      d->m_drag_state = DraggingMousePressed;
    } else {
      e->ignore();
    }
    setChecked(!isChecked());
    Q_EMIT clicked();
    return;
  }
  Super::mousePressEvent(e);
}

void IWidgetTab::mouseReleaseEvent(QMouseEvent *e) {
  if (isCanSpliting() && e->button() == Qt::LeftButton) {
    auto CurrentDragState = d->m_drag_state;
    d->m_global_drag_start_mouse_position = QPoint();
    d->m_drag_start_mouse_position = QPoint();
    d->m_drag_state = DraggingInactive;

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
  if (isCanSpliting()) {
    if (!(e->buttons() & Qt::LeftButton) ||
        d->isDraggingState(DraggingInactive)) {
      d->m_drag_state = DraggingInactive;
      Super::mouseMoveEvent(e);
      return;
    }

    QPoint point = e->pos() - d->m_drag_start_mouse_position;
    if (d->isDraggingState(DraggingMousePressed) &&
        point.manhattanLength() > 10) {
      if (d->m_tab_info->m_tagWidget) {
        d->m_tab_info->m_tagWidget->startSplits();
        d->m_drag_state = DraggingTab;
        Q_EMIT tabsplit();
      }
    }
  }

  Super::mouseMoveEvent(e);
}

void IWidgetTab::contextMenuEvent(QContextMenuEvent *e) {}

WidgetTab::WidgetTab(TabInfoStruct *tab, QWidget *parent) : Super(tab, parent) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(15, 0, 15, 0);
  layout->setSpacing(5);

  QLabel *title = new QLabel(titleName());
  title->setMouseTracking(true);
  layout->addWidget(title);

  m_close_btn = new QPushButton;
  m_close_btn->setFixedSize(20, 20);
  m_close_btn->setIcon(getIcon());
  // close_btn->setIconSize(QSize(8, 8));
  connect(m_close_btn, &QPushButton::clicked, this, &WidgetTab::closed);
  layout->addWidget(m_close_btn);
  setChecked(true);
}

WidgetTab::~WidgetTab() {}

void WidgetTab::setCanClosed(bool canClosed) {
  m_close_btn->setVisible(canClosed);
}

bool WidgetTab::isCanClosed() { return m_close_btn->isVisible(); }

void WidgetTab::paintEvent(QPaintEvent *e) {
  Q_UNUSED(e);
  QPainter painter(this);
  painter.setFont(font());
  painter.setPen(Qt::NoPen);
  painter.fillRect(rect(),
                   QBrush(isChecked() ? QColor("#FFFFFF") : QColor("#E0E5ED")));
}
