#-------------------------------------------------
#
# Project created by QtCreator 2015-07-28T12:06:11
#
#-------------------------------------------------

QT       += core gui network x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tianqi
TEMPLATE = app

LIBS += -lX11 -lXext

SOURCES += main.cpp\
        tianqi.cpp \
    search.cpp \
    setting.cpp

HEADERS  += tianqi.h \
    search.h \
    setting.h

FORMS    += tianqi.ui \
    search.ui \
    setting.ui

RESOURCES += \
    icon.qrc
