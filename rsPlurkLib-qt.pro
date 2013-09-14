#-------------------------------------------------
#
# Project created by QtCreator 2013-09-14T22:03:48
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = rsPlurkLib-qt
TEMPLATE = lib
CONFIG += staticlib

SOURCES += rsplurkclient.cpp

HEADERS += rsplurkclient.h 

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
