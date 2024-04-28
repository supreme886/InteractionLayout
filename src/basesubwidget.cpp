#include "basesubwidget.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

#include "drageventfilter.h"
#include "dragmanager.h"
#include "floatingwidgetcontainer.h"
#include "global.h"
#include "interactionlayout.h"
#include "widgetresizehandler.h"
#include "widgettab.h"

struct BaseWidgetPrivate {
  BaseWidgetPrivate(BaseSubWidget *pub)
      : m_this(pub), m_widget_tab(new TabInfoStruct(m_this)) {}

  void saveDragStartMousePosition(const QPoint &global_pos);

  TabInfoStruct *getTabInfo();

  void setResizeHandle(bool has_resizer);
  void setCanResize(bool can_resize);
  bool isCanResize();
  void setCanClosed(bool canClosed);
  bool isCanClosed();

  BaseSubWidget *m_this{nullptr};
  TabInfoStruct *m_widget_tab{nullptr};
  WidgetResizeHandler *m_reszie_handler{nullptr};
  QPoint m_press_point;
  QPoint m_press_global_point;
  bool m_has_press{false};
  bool m_has_container{false};
  bool m_can_resize{true};
};

void BaseWidgetPrivate::saveDragStartMousePosition(const QPoint &global_pos) {
  m_press_point = global_pos;
  m_press_global_point = m_this->mapFromGlobal(global_pos);
}

TabInfoStruct *BaseWidgetPrivate::getTabInfo() { return m_widget_tab; }

void BaseWidgetPrivate::setResizeHandle(bool has_resizer) {
  if (!m_reszie_handler) {
    m_reszie_handler = new WidgetResizeHandler(m_this);
    m_reszie_handler->setActive(WidgetResizeHandler::Move, false);
  }
  // d->m_reszie_handler->setMovingEnabled(canResize);
  m_reszie_handler->setActive(WidgetResizeHandler::Resize, has_resizer);
}

void BaseWidgetPrivate::setCanResize(bool can_resize) {
  m_can_resize = can_resize;
}

bool BaseWidgetPrivate::isCanResize() { return m_can_resize; }

void BaseWidgetPrivate::setCanClosed(bool can_closed) {
  m_widget_tab->m_canClosed = can_closed;
}

bool BaseWidgetPrivate::isCanClosed() { return m_widget_tab->m_canClosed; }

BaseSubWidget::BaseSubWidget(QWidget *parent)
    : Super(parent), d(new BaseWidgetPrivate(this)) {
  d->m_widget_tab->m_titleName = "dadadasdddddddddddddd";
  setMouseTracking(true);
  setMinimumSize(200, 200);
}

BaseSubWidget::~BaseSubWidget() {
  qDebug() << Q_FUNC_INFO;
  unPlugSelf();
}

FloatingWidgetContainer *BaseSubWidget::startSplits() {
  QRect r = geometry();
  r.moveTopLeft(mapToGlobal(QPoint(0, 30)));

  unPlugSelf();

  FloatingWidgetContainer *container = new FloatingWidgetContainer(this);
  container->setGeometry(r);
  container->setWindowTitle(d->m_widget_tab->m_titleName);
  container->show();

  DragManager::instance()->setDragEventFilter(container);
  QMouseEvent *mouse_event =
      new QMouseEvent(QEvent::MouseButtonPress, QCursor::pos(), Qt::LeftButton,
                      Qt::LeftButton, Qt::NoModifier);
  qApp->sendEvent(container, mouse_event);

  return container;
}

void BaseSubWidget::endSplits() {}

void BaseSubWidget::setResizeHandle(bool can_resize) {
  d->setResizeHandle(can_resize);
}

TabInfoStruct *BaseSubWidget::getTabInfoStruct() { return d->m_widget_tab; }

void BaseSubWidget::setCanspliting(bool can_spliting) {
  getTabInfoStruct()->m_canSpliting = can_spliting;
}

bool BaseSubWidget::isCanSpliting() {
  return getTabInfoStruct()->m_canSpliting;
}

void BaseSubWidget::setCanResize(bool can_resize) {
  d->setCanResize(can_resize);
}

bool BaseSubWidget::isCanResize() { return d->isCanResize(); }

void BaseSubWidget::setCanClosed(bool can_closed) {
  d->setCanClosed(can_closed);
}

bool BaseSubWidget::isCanClosed() { return d->isCanClosed(); }

bool BaseSubWidget::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::WindowTitleChange:
      d->m_widget_tab->m_titleName = windowTitle();
      break;
    default:
      break;
  }
  return Super::event(event);
}

void BaseSubWidget::mousePressEvent(QMouseEvent *e) {
  if (Qt::LeftButton == e->button()) {
    d->m_has_press = true;
    d->saveDragStartMousePosition(e->pos());
    grabMouse();
  }
  Super::mousePressEvent(e);
}

void BaseSubWidget::mouseReleaseEvent(QMouseEvent *e) {
  if (Qt::LeftButton == e->button()) {
    d->m_has_press = false;
    d->m_press_point = QPoint();
    d->m_press_global_point = QPoint();
    releaseMouse();
  }
  Super::mouseReleaseEvent(e);
}

void BaseSubWidget::mouseMoveEvent(QMouseEvent *e) {
  bool can_move =
      QPoint(e->globalPos() - d->m_press_global_point).manhattanLength() > 6;
  if (d->m_has_press && can_move) {
    // startSplits();
  }
  Super::mouseMoveEvent(e);
}

void BaseSubWidget::resizeEvent(QResizeEvent *e) {
  QWidget *pw = qobject_cast<QWidget *>(parent());
  if (pw) {
    InteractionLayout *layout = dynamic_cast<InteractionLayout *>(pw->layout());
    if (layout) {
      layout->invalidate();
    }
  }
  Super::resizeEvent(e);
}

void BaseSubWidget::unPlugSelf() {
  QWidget *pw = qobject_cast<QWidget *>(parent());
  if (pw) {
    InteractionLayout *layout = dynamic_cast<InteractionLayout *>(pw->layout());
    if (layout) {
      layout->unplug(this);
      setResizeHandle(false);
    }
  }
}
