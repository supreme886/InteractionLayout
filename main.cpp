#include <QApplication>
#include <QMainWindow>

#include "widget.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QMainWindow mainWidget;
  mainWidget.resize(1000, 800);
  Widget *w = new Widget;
  w->setWindowTitle("this is Widget");
  // w->show();

  mainWidget.setCentralWidget(w);
  mainWidget.show();

  // Widget w1;
  // w1.setWindowTitle("this is Widget_1");
  // w1.show();
  return a.exec();
}
