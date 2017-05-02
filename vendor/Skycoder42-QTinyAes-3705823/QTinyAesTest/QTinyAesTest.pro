#-------------------------------------------------
#
# Project created by QtCreator 2015-09-28T20:58:51
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_qtinyaestest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../vendor/vendor.pri)
include(../de_skycoder42_qtinyaes.pri)

SOURCES += tst_qtinyaestest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
