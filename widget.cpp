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

#include "src/basewidget.h"
#include "src/dragmanager.h"
#include "src/floatingwidgetcontainer.h"
#include "src/interactionlayout.h"
#include "src/iwidget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);

  InteractionLayout *layout = new InteractionLayout(this);
  layout->setContentsMargins(10, 0, 0, 0);

  BaseSubWidget *tw = new BaseSubWidget(this);
  tw->resize(200, 300);
  tw->setStyleSheet("background: red;");
  layout->addWidgetByArea(Top_Area, tw);

  BaseSubWidget *rw = new BaseSubWidget(this);
  rw->resize(200, 300);
  rw->setStyleSheet("background: green;");
  layout->addWidgetByArea(Right_Area, rw);

  BaseSubWidget *bw = new BaseSubWidget(this);
  bw->resize(200, 300);
  bw->setStyleSheet("background: lightgray;");
  layout->addWidgetByArea(Bottom_Area, bw);

  BaseSubWidget *lw = new BaseSubWidget(this);
  lw->resize(100, 200);
  lw->setStyleSheet("background: blue;");
  layout->addWidgetByArea(Left_Area, lw);

  QVBoxLayout *l = new QVBoxLayout(lw);
  l->setContentsMargins(10, 10, 10, 10);
  l->setSpacing(2);
  l->addWidget(new QLabel("QLabel 1"));
  l->addWidget(new QLabel("QLabel 1"));
  l->addWidget(new QPushButton("QPushButton"));
  l->addWidget(new QLabel("QLabel 1"));
  l->addWidget(new QLabel("QLabel 1"));

  QHBoxLayout *ilayout = new QHBoxLayout(lw);
  ilayout->addWidget(new QLabel(windowTitle()));

  QWidget *gw = new QWidget;
  gw->setStyleSheet("background: back;");
  layout->addWidget(gw);

  setLayout(layout);
}

Widget::~Widget() { delete ui; }

void Widget::paintEvent(QPaintEvent *e) {
  Q_UNUSED(e)

  QPainter painter(this);
  painter.fillRect(rect(), QBrush(Qt::gray));
}
