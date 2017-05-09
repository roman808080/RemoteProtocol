#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QDataStream>
#include <QThread>
#include <QList>
#include <QScopedPointer>
#include <QSharedPointer>

#include <iostream>
#include <windows.h>

#include "tcpprotocol.h"
#include "convertordata.h"
#include "datastruct.h"
#include "connectionhandler.h"
#include "processhandler.h"

class App : public QObject
{
    Q_OBJECT

public:
    App();
    virtual ~App();
    void menu();
    QString publicIp();

private:
    TcpProtocol  remoteProtocol;
    ProcessHandler processHandler;
};

#endif // EXAMPLE_H
