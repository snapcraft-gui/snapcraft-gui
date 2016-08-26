#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T23:50:55
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = snapcraft-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    highlighter.cpp \
    install_local_snap_dialog.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    install_local_snap_dialog.h

FORMS    += mainwindow.ui \
    install_local_snap_dialog.ui

RESOURCES += \
    rc.qrc
