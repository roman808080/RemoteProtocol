QT += core
QT -= gui

CONFIG += c++11
QT += network

TARGET = remoteprotocol
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    example.cpp \
    udpprotocol.cpp \
    tcpprotocol.cpp \
    convertordata.cpp \
    console.cpp \
    connectionhandler.cpp

HEADERS += \
    peer.h \
    example.h \
    udpprotocol.h \
    tcpprotocol.h \
    convertordata.h \
    datastruct.h \
    inputconsole.h \
    console.h \
    connectionhandler.h

include(vendor/vendor.pri)
