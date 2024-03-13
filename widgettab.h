#ifndef WIDGETTAB_H
#define WIDGETTAB_H

#include <QFrame>
#include <QObject>

class WidgetTab : public QFrame {
  Q_OBJECT
 public:
  using Supre = QFrame;
  WidgetTab(QWidget *parent);
};

#endif  // WIDGETTAB_H
