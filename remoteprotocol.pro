QT += core
QT -= gui

CONFIG += c++11
QT += network
QT += widgets

TARGET = remoteprotocol
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    tcpprotocol.cpp \
    convertordata.cpp \
    console.cpp \
    connectionhandler.cpp \
    app.cpp

HEADERS += \
    convertordata.h \
    datastruct.h \
    console.h \
    connectionhandler.h \
    ulong.h \
    diffiehellmankeysexchanger.h \
    randomkeygenerator.h \
    processhandler.h \
    app.h \
    tcpprotocol.h

include(vendor/vendor.pri)
