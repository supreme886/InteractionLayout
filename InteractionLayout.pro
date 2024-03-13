QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets widgets-private

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dragdecorator.cpp \
    dragmanager.cpp \
    floatingwidgetcontainer.cpp \
    interactionlayout.cpp \
    iwidget.cpp \
    main.cpp \
    widget.cpp \
    widgetgroup.cpp \
    widgettab.cpp

HEADERS += \
    dragdecorator.h \
    dragmanager.h \
    floatingwidgetcontainer.h \
    global.h \
    interactionlayout.h \
    iwidget.h \
    widget.h \
    widgetgroup.h \
    widgettab.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
