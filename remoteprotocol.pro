QT += core
QT -= gui

CONFIG += c++11
QT += network

TARGET = remoteprotocol
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    remoteprotocol.cpp \
    example.cpp

HEADERS += \
    remoteprotocol.h \
    peer.h \
    example.h
