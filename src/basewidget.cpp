#include "basewidget.h"

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
      : _this(pub), m_widgetTab(new TabInfoStruct(_this)) {}

  void saveDragStartMousePosition(const QPoint &GlobalPos);

  TabInfoStruct *getTabInfo();

  void setResizeHandle(bool canResize);
  void setCanResize(bool canResize);
  bool isCanResize();

  BaseSubWidget *_this{nullptr};
  TabInfoStruct *m_widgetTab{nullptr};
  WidgetResizeHandler *m_reszie_handler{nullptr};
  QPoint m_press_point;
  QPoint m_press_global_point;
  bool m_has_press{false};
  bool m_has_container{false};
  bool m_can_resize{true};
};

void BaseWidgetPrivate::saveDragStartMousePosition(const QPoint &GlobalPos) {
  m_press_point = GlobalPos;
  m_press_global_point = _this->mapFromGlobal(GlobalPos);
}

TabInfoStruct *BaseWidgetPrivate::getTabInfo() { return m_widgetTab; }

void BaseWidgetPrivate::setResizeHandle(bool canResize) {
  if (!m_reszie_handler) {
    m_reszie_handler = new WidgetResizeHandler(_this);
    m_reszie_handler->setActive(WidgetResizeHandler::Move, false);
  }
  // d->m_reszie_handler->setMovingEnabled(canResize);
  m_reszie_handler->setActive(WidgetResizeHandler::Resize, canResize);
}

void BaseWidgetPrivate::setCanResize(bool canResize) {
  m_can_resize = canResize;
}

bool BaseWidgetPrivate::isCanResize() { return m_can_resize; }

BaseSubWidget::BaseSubWidget(QWidget *parent)
    : Super(parent), d(new BaseWidgetPrivate(this)) {
  d->m_widgetTab->m_titleName = "dadadasdddddddddddddd";
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
  container->setWindowTitle(d->m_widgetTab->m_titleName);
  container->show();

  DragManager::instance()->setDragEventFilter(container);
  QMouseEvent *mouseEvent =
      new QMouseEvent(QEvent::MouseButtonPress, QCursor::pos(), Qt::LeftButton,
                      Qt::LeftButton, Qt::NoModifier);
  qApp->sendEvent(container, mouseEvent);

  return container;
}

void BaseSubWidget::endSplits() {}

void BaseSubWidget::setResizeHandle(bool canResize) {
  d->setResizeHandle(canResize);
}

TabInfoStruct *BaseSubWidget::getTabInfoStruct() { return d->m_widgetTab; }

void BaseSubWidget::setCanspliting(bool canSpliting) {
  getTabInfoStruct()->m_canSpliting = canSpliting;
}

bool BaseSubWidget::isCanSpliting() {
  return getTabInfoStruct()->m_canSpliting;
}

void BaseSubWidget::setCanResize(bool canResize) { d->setCanResize(canResize); }

bool BaseSubWidget::isCanResize() { return d->isCanResize(); }

bool BaseSubWidget::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::WindowTitleChange:
      d->m_widgetTab->m_titleName = windowTitle();
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
