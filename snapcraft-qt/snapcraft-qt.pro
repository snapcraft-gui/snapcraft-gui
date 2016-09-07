#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T23:50:55
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = snapcraft-gui
TEMPLATE = app

target.path = /opt/snapcraft-gui/
INSTALLS += target

SOURCES += main.cpp\
        mainwindow.cpp \
    highlighter.cpp \
    install_local_snap_dialog.cpp \
    store.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    install_local_snap_dialog.h \
    store.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    install_local_snap_dialog.ui \
    store.ui \
    clean_dialog.ui \
    aboutdialog.ui \
    pull_dialog.ui \
    stage_dialog.ui \
    prime_dialog.ui \
    build_dialog.ui

RESOURCES += \
    rc.qrc
