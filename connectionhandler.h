#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include <exception>

#include "datastruct.h"
#include "console.h"
#include "convertordata.h"

class ConnectionHandler:QObject
{
    Q_OBJECT
public:
    ConnectionHandler(QSharedPointer<QTcpSocket> socket);
    ~ConnectionHandler();

    void startServer();

public slots:
    void sendConnectError(QAbstractSocket::SocketError e);
    void closedConnection();

    void readyRead();

private:
    // client side
    int write(DataIn& data);
    int read(DataOut& data);

    //server side
    int write(DataOut& data);
    int read(DataIn& data);

    qint32 readBufferSize;
    QSharedPointer<QTcpSocket> socket;
//    Console console;
};

#endif // CONNECTIONHANDLER_H
