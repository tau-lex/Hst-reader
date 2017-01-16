#-------------------------------------------------
#
# Project created by QtCreator 2016-10-25T19:08:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hst-reader
TEMPLATE = app


SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/imt4reader.cpp \
    src/hstreader.cpp \
    src/csvreader.cpp \
    src/csvwriter.cpp

HEADERS  += include/mainwindow.h \
    include/imt4reader.h \
    include/hstreader.h \
    include/csvreader.h \
    include/csvwriter.h

FORMS    += mainwindow.ui

win32-g++{
    QMAKE_LFLAGS += -static-libgcc
    QMAKE_LFLAGS += -static-libstdc++
    QMAKE_LFLAGS += -static
}
