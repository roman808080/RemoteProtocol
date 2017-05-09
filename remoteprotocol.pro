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
    udpprotocol.cpp \
    tcpprotocol.cpp \
    convertordata.cpp \
    console.cpp \
    connectionhandler.cpp \
    processhandler.cpp \
    app.cpp

HEADERS += \
    peer.h \
    udpprotocol.h \
    tcpprotocol.h \
    convertordata.h \
    datastruct.h \
    console.h \
    connectionhandler.h \
    ulong.h \
    diffiehellmankeysexchanger.h \
    randomkeygenerator.h \
    processhandler.h \
    app.h

include(vendor/vendor.pri)
