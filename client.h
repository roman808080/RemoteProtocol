#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QSharedPointer>
#include <QDataStream>
#include <QDebug>
#include <QByteArray>

#include <iostream>

#include "convertordata.h"
#include "datastruct.h"
#include "console.h"

class Client: QObject//: public QThread
{
    Q_OBJECT
public:
    Client(QSharedPointer<QTcpSocket> socket);
    ~Client();

public slots:
    void sendConnectError(QAbstractSocket::SocketError e);
    void closedConnection();

    void startExchange();
    void endExchange();

private:
    int write(DataIn& data);
    int read(DataOut& data);

    qint32 size;
    QSharedPointer<QTcpSocket> socket;
    Console console;
};

#endif // CLIENT_H
