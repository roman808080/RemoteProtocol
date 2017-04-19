#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <QObject>
#include <QTcpSocket>
#include <QSharedPointer>
#include <iostream>
#include "tcpprotocol.h"

#include "peer.h"
#include "server.h"
#include "client.h"
#include "datastruct.h"

class Example : public QObject
{
    Q_OBJECT

public:
    Example();
    virtual ~Example();
    void menu();
    void sayHello();
    void createClient(std::__cxx11::string ip, int port);
    void createServer();

public slots:
    void newUsers(Peer peer);
    void newUsers();
    void sendConnectError(QAbstractSocket::SocketError);

    void newInConnection(QSharedPointer<QTcpSocket> socket);
    void newOutConnection(QSharedPointer<QTcpSocket> socket);

private:
    TcpProtocol  remoteProtocol;
//    QList<QScopedPointer<Client>> listClients;
//    QList<QScopedPointer<Server>> listServers;
    QScopedPointer<Client> client;
    QScopedPointer<Server> server;
};

#endif // EXAMPLE_H
