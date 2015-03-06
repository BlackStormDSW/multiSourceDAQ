#-------------------------------------------------
#
# Project created by QtCreator 2015-01-20T21:27:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = multiSourceDAQ
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qextserialbase.cpp \
    qextserialport.cpp \
    win_qextserialport.cpp \
    readdata.cpp \
    senddata.cpp \
    inputdata.cpp \
    outputdata.cpp

HEADERS  += mainwindow.h \
    qextserialbase.h \
    qextserialport.h \
    win_qextserialport.h \
    readdata.h \
    senddata.h \
    dataprotocol.h \
    inputdata.h \
    outputdata.h

FORMS    += mainwindow.ui
