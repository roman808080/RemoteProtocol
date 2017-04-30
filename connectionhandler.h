#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#include <QObject>
//#include <QTcpSocket>
#include <QSslSocket>
#include <QDataStream>

#include <exception>

#include "datastruct.h"
#include "console.h"
#include "convertordata.h"

class ConnectionHandler:public QObject
{
    Q_OBJECT
public:
    ConnectionHandler();
    ConnectionHandler(QSharedPointer<QSslSocket> socket);
    ~ConnectionHandler();

    void startServer();
    void startClient();
    void setSocket(QSharedPointer<QSslSocket> socket);

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
    QSharedPointer<QSslSocket> socket;
    Console console;
    qint32 type;
};

#endif // CONNECTIONHANDLER_H
