#include <QApplication>

#include "widget.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Widget w;
  w.setWindowTitle("this is Widget");
  w.show();
  // Widget w1;
  // w1.setWindowTitle("this is Widget_1");
  // w1.show();
  return a.exec();
}
