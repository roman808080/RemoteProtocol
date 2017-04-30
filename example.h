#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QDataStream>
#include <QThread>

#include <iostream>
#include <windows.h>

#include "tcpprotocol.h"
#include "peer.h"
#include "convertordata.h"
#include "datastruct.h"
#include "connectionhandler.h"

class Example : public QObject
{
    Q_OBJECT

public:
    Example();
    virtual ~Example();
    void menu();
    void createClient(std::string ip, int port);
    void createServer();

public slots:
    void newInConnection(QSharedPointer<QTcpSocket> socket);
    void newOutConnection(QSharedPointer<QTcpSocket> socket);

private:
    TcpProtocol  remoteProtocol;
//    QScopedPointer<ConnectionHandler> connectionHandler;
    ConnectionHandler connectionHandler;
};

#endif // EXAMPLE_H
