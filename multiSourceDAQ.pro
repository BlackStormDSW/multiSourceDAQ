##########################################################################
##  All rights reserved by Yantai XTD test technology co., LTD.         ##
##									##
##                          Author: Dong Shengwei			##
##                          Date: 2015-01-20                            ##
##########################################################################

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = multiSourceDAQ
TEMPLATE = app

LIBS += -lAdvapi32

SOURCES += main.cpp\
        mainwindow.cpp \
    qextserialbase.cpp \
    qextserialport.cpp \
    win_qextserialport.cpp \
    inputdata.cpp \
    outputdata.cpp \
    combobox.cpp

HEADERS  += mainwindow.h \
    qextserialbase.h \
    qextserialport.h \
    win_qextserialport.h \
    dataprotocol.h \
    inputdata.h \
    outputdata.h \
    combobox.h

FORMS    +=

RC_FILE = multiSourceDAQ.rc
