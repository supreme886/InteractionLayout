#include "widgettab.h"

struct WidgetTabPrivate {
  WidgetTabPrivate(QWidget *tagWidget) : m_tagWidget(tagWidget) {}

  QWidget *m_tagWidget{nullptr};
};

WidgetTab::WidgetTab(QWidget *tagWidget, QWidget *parent)
    : Supre(parent), d(new WidgetTabPrivate(tagWidget)) {}

void WidgetTab::mousePressEvent(QMouseEvent *ev)
{

}

void WidgetTab::mouseReleaseEvent(QMouseEvent *ev)
{

}

void WidgetTab::mouseMoveEvent(QMouseEvent *ev)
{

}

void WidgetTab::contextMenuEvent(QContextMenuEvent *ev)
{

}
