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
    highlighter.cpp

HEADERS  += mainwindow.h \
    highlighter.h

FORMS    += mainwindow.ui

RESOURCES += \
    rc.qrc
