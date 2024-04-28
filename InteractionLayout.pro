QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/basesubwidget.cpp \
    src/drageventfilter.cpp \
    src/dragmanager.cpp \
    src/floatingwidgetcontainer.cpp \
    src/interactionlayout.cpp \
    # src/iwidget.cpp \
    main.cpp \
    widget.cpp \
    src/widgetgroup.cpp \
    src/widgettab.cpp \
    src/widgettabbar.cpp \
    src/widgetresizehandler.cpp

HEADERS += \
    src/basesubwidget.h \
    src/drageventfilter.h \
    src/dragmanager.h \
    src/floatingwidgetcontainer.h \
    src/global.h \
    src/interactionlayout.h \
    src/interface.h \
    # src/iwidget.h \
    widget.h \
    src/widgetgroup.h \
    src/widgettab.h \
    src/widgettabbar.h \
    src/widgetresizehandler.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
