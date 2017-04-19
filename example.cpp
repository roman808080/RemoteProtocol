#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <iostream>
#include <windows.h>
#include <QThread>

#include "example.h"
#include "convertordata.h"
#include "datastruct.h"
#include "client.h"
#include "server.h"

Example::Example(){
      qDebug()<<"From worker thread: "<< QThread::currentThreadId();
//    connect(&remoteProtocol, SIGNAL(peerListAdded(Peer)), this, SLOT(newUsers(Peer)), Qt::DirectConnection);
    QObject::connect(&remoteProtocol, &TcpProtocol::newInConnection, this, &Example::newInConnection, Qt::QueuedConnection);
//    QObject::connect(&remoteProtocol, &TcpProtocol::newOutConnection, this, &Example::newOutConnection, Qt::DirectConnection);
//    QObject::connect(&remoteProtocol, &RemoteProtocol::receiveTextComplete, this, &Example::receiveTextComplete, Qt::DirectConnection);
}

Example::~Example(){

}
void Example::newUsers(Peer peer){
    std::cout << "We have new user\n";
}

void Example::newUsers(){
    std::cout << "We have new user\n";
}

 void Example::newInConnection(QSharedPointer<QTcpSocket> socket){
     connect(socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
     std::cout << "Server have new connection from client\n";
//     QSharedPointer<Server> server(socket);
//     server->loop();
//     listServers.append(server);
//     listServers.append(new QScopedPointer<Server>(new Server(socket)));
     server.reset(new Server(socket));
     server->loop();
 }

 void Example::newOutConnection(QSharedPointer<QTcpSocket> socket){
     std::cout << "Client have new connection with server\n";
//     QSharedPointer<Client> client(socket);
//     listClients.append(new QScopedPointer<Client>(new Client(socket)));
//     client->loop();
//     listClients.append(*client);
     client.reset(new Client(socket));
     client->loop();
 }


void Example::menu(){
    int choice;

    std::cout << "Hello in menu\n ";
    std::cout << "Input 1 for create Server\n";
    std::cout << "Input 2 for create Client\n";
    std::cout << "Input -1 for end program\n";

    std::cin >> choice;

    if(choice == 1){
        std::cout << "You're choice is server\n";
        remoteProtocol.runTcpServer();
    }
    else if(choice == 2){
        std::string ip;
        int port;

        std::cout << "You're choice is client\n";

        std::cout << "Please input ip or -1 if you'll want to use local 127.0.0.1 ip\n";
        std::cin >> ip;

        std::cout << "Please input port or -1 if you'll want to use standart 4644 port\n";
        std::cin >> port;

        createClient(ip, port);
    }
    else {
        std::cout << "Invalid comand\n";
    }

}

void Example::createClient(std::string ip, int port){
    if(ip == "-1")
        ip = "127.0.0.1";
    if(port == -1)
        port = 4644;
    QObject::connect(&remoteProtocol, &TcpProtocol::newOutConnection, this, &Example::newOutConnection, Qt::DirectConnection);
    remoteProtocol.newOutcomingConnection(QString::fromUtf8(ip.c_str()), port);
}

void Example::createServer(){
//    QObject::connect(&remoteProtocol, &TcpProtocol::newOutConnection, this, &Example::newOutConnection, Qt::DirectConnection);
    remoteProtocol.runTcpServer();
//    connect(&remoteProtocol, SIGNAL(newClientConnection()), this, SLOT(newClientConnection()), Qt::DirectConnection);
//    QObject::connect(&remoteProtocol, &RemoteProtocol::receiveTextComplete, this, &Example::receiveTextComplete);
}

void Example::sayHello(){
//    remoteProtocol.sayHello(QHostAddress::Broadcast, true);
}

void Example::sendConnectError(QAbstractSocket::SocketError e)
{
//    if (mCurrentSocket)
//    {
//        mCurrentSocket->close();
//        mCurrentSocket->deleteLater();
//    }
}
