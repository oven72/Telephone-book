#-------------------------------------------------
#
# Project created by QtCreator 2015-09-16T13:25:28
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tels
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    history.cpp \
    add_user.cpp \
    add_tel.cpp

HEADERS  += mainwindow.h \
    history.h \
    add_user.h \
    add_tel.h

FORMS    += mainwindow.ui \
    history.ui \
    add_user.ui \
    add_tel.ui

RESOURCES += \
    res.qrc
