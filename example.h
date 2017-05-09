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
#include "peer.h"
#include "convertordata.h"
#include "datastruct.h"
#include "connectionhandler.h"
#include "processhandler.h"

class Example : public QObject
{
    Q_OBJECT

public:
    Example();
    virtual ~Example();
    void menu();

private:
    TcpProtocol  remoteProtocol;
    ProcessHandler processHandler;
};

#endif // EXAMPLE_H
