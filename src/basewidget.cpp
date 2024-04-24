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

  BaseSubWidget *_this{nullptr};
  TabInfoStruct *m_widgetTab{nullptr};
  WidgetResizeHandler *m_reszie_handler{nullptr};
  QPoint m_press_point;
  QPoint m_press_global_point;
  bool m_has_press{false};
  bool m_has_container{false};
};

void BaseWidgetPrivate::saveDragStartMousePosition(const QPoint &GlobalPos) {
  m_press_point = GlobalPos;
  m_press_global_point = _this->mapFromGlobal(GlobalPos);
}

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
  if (!d->m_reszie_handler) {
    d->m_reszie_handler = new WidgetResizeHandler(this);
    d->m_reszie_handler->setActive(WidgetResizeHandler::Move, false);
  }
  // d->m_reszie_handler->setMovingEnabled(canResize);
  d->m_reszie_handler->setActive(WidgetResizeHandler::Resize, canResize);
}

TabInfoStruct *BaseSubWidget::getTabInfoStruct() { return d->m_widgetTab; }

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
  // QWidget *pw = qobject_cast<QWidget *>(parent());
  // if (pw) {
  //   InteractionLayout *layout = dynamic_cast<InteractionLayout
  //   *>(pw->layout()); if (layout) {
  //     layout->invalidate();
  //   }
  // }
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

// void BaseSubWidget::paintEvent(QPaintEvent *event) {
// Super::paintEvent(event); }
