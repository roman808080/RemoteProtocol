
DEFINES += QPM_INIT\\(E\\)=\"E.addImportPath(QStringLiteral(\\\"qrc:/\\\"));\"
DEFINES += QPM_USE_NS
INCLUDEPATH += $$PWD
QML_IMPORT_PATH += $$PWD


include($$PWD/de\skycoder42\qtinyaes\de_skycoder42_qtinyaes.pri)
include($$PWD/de\skycoder42\kokke\tiny-aes128-c\de_skycoder42_kokke_tiny-aes128-c.pri)
