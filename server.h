#ifndef SERVER_H
#define SERVER_H

#include <QSharedPointer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QThread>

#include "convertordata.h"
#include "datastruct.h"
#include "serverconsole.h"

class Server: public QObject//: public QThread
{
   Q_OBJECT
public:
    Server(QSharedPointer<QTcpSocket> socket);
    ~Server();

public slots:
    void sendConnectError(QAbstractSocket::SocketError e);
    void closedConnection();
    void exchange();

private:
    int write(DataOut& data);
    int read(DataIn& data);

    QSharedPointer<QTcpSocket> socket;
};

#endif // SERVER_H
