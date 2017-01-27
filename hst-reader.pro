###############################################################################
#                                                                             #
#   Hst to Csv Converter Project                                              #
#   web.site.com                                                              #
#                                                                             #
#   H S T - R E A D E R   P R O G R A M                                       #
#                                                                             #
#   Aleksey Terentew                                                          #
#   terentew.aleksey@ya.ru                                                    #
#                                                                             #
###############################################################################

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hst-reader
TEMPLATE = app


SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/imt4reader.cpp \
    src/hstreader.cpp \
    src/csvreader.cpp \
    src/csvwriter.cpp \
    src/csvpredictionwriter.cpp

HEADERS  += include/mainwindow.h \
    include/imt4reader.h \
    include/hstreader.h \
    include/csvreader.h \
    include/csvwriter.h \
    include/csvpredictionwriter.h

FORMS    += mainwindow.ui

win32-g++{
    QMAKE_LFLAGS += -static-libgcc
    QMAKE_LFLAGS += -static-libstdc++
    QMAKE_LFLAGS += -static
}
