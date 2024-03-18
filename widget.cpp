#include "widget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QMdiSubWindow>
#include <QPainter>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>

#include "basewidget.h"
#include "dragmanager.h"
#include "floatingwidgetcontainer.h"
#include "interactionlayout.h"
#include "iwidget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);

  InteractionLayout *layout = new InteractionLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);

  BaseWidget *tw = new BaseWidget(this);
  tw->resize(200, 300);
  tw->setStyleSheet("background: red;");
  layout->addWidgetByArea(LayoutInterface::Top_Area, tw);

  IWidget *rw = new IWidget(this);
  rw->resize(200, 300);
  rw->setStyleSheet("background: green;");
  layout->addWidgetByArea(LayoutInterface::Right_Area, rw);

  QWidget *bw = new QWidget(this);
  bw->resize(200, 300);
  bw->setStyleSheet("background: lightgray;");
  layout->addWidgetByArea(LayoutInterface::Bottom_Area, bw);

  IWidget *lw = new IWidget(this);
  lw->resize(100, 200);
  lw->setStyleSheet("background: blue;");
  layout->addWidgetByArea(LayoutInterface::Left_Area, lw);

  QHBoxLayout *ilayout = new QHBoxLayout(lw);
  ilayout->addWidget(new QLabel(windowTitle()));

  // FloatingWidgetContainer *floating =
  //     new FloatingWidgetContainer(new QPushButton);
  // floating->show();
  // floating->raise();

  // QVBoxLayout *a = new QVBoxLayout(lw);
  // a->setMargin(0);
  // auto tab_widget = new QTabWidget;
  // a->addWidget(tab_widget);

  // QWidget *one_widget = new QWidget;
  // QVBoxLayout *b = new QVBoxLayout(one_widget);
  // b->setMargin(0);
  // b->addWidget(new QTextEdit);

  // tab_widget->addTab(one_widget, "one");
  // tab_widget->addTab(new QWidget, "two");
  // tab_widget->addTab(new QWidget, "three");

  QWidget *gw = new QWidget;
  gw->resize(10, 10);
  gw->setStyleSheet("background: green;");
  layout->addWidget(gw);

  setLayout(layout);
}

Widget::~Widget() { delete ui; }

void Widget::paintEvent(QPaintEvent *e) {
  Q_UNUSED(e)

  QPainter painter(this);
  painter.fillRect(rect(), QBrush(Qt::gray));
}
