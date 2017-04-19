#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <QObject>
#include <QTcpSocket>
#include <QSharedPointer>
#include <iostream>
#include "tcpprotocol.h"

#include "peer.h"
struct DataIn;
struct DataOut;

class Example : public QObject
{
    Q_OBJECT
//    Q_DECLARE_METATYPE(std::string)
public:
    Example();
    virtual ~Example();
    void menu();
    void sayHello();
    void createClient(std::__cxx11::string ip, int port);
    void createServer();
    int write(QSharedPointer<QTcpSocket> socket, DataIn &data);
    int read(QSharedPointer<QTcpSocket> socket, DataIn &data);

    int write(QSharedPointer<QTcpSocket> socket, DataOut& data);
    int read(QSharedPointer<QTcpSocket> socket, DataOut &data);

public slots:
    void newUsers(Peer peer);
    void newUsers();
    void sendConnectError(QAbstractSocket::SocketError);

    void newInConnection(QSharedPointer<QTcpSocket> socket);
    void newOutConnection(QSharedPointer<QTcpSocket> socket);

private:
//    QDataStream in;
    TcpProtocol  remoteProtocol;
};

#endif // EXAMPLE_H
