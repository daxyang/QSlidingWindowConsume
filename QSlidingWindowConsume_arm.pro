#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T16:55:38
#
#-------------------------------------------------

QT       -= gui

TARGET = QSlidingWindowConsume
TEMPLATE = lib

DEFINES += QSLIDINGWINDOWCONSUME_LIBRARY

SOURCES += qslidingwindowconsume.cpp

HEADERS += qslidingwindowconsume.h\
        qslidingwindowconsume_global.h
#ubuntu
#unix:!macx {
#    target.path = /usr/local/linux_lib/lib
#    INSTALLS += target
#}
#arm
unix:!macx {
    target.path = /usr/local/arm_lib
    INSTALLS += target
}
unix:macx{
    target.path = /usr/local/lib
    INSTALLS += target
}
