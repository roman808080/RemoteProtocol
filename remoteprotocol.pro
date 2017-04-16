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
    tcpprotocol.cpp

HEADERS += \
    peer.h \
    example.h \
    udpprotocol.h \
    tcpprotocol.h
