#include <QObject>
#include <iostream>

#include "example.h"

Example::Example(){
//    remoteProtocol.runTcpServer();
//    remoteProtocol.runUdpSocket();
//    remoteProtocol.sayHello(QHostAddress::Broadcast, true);

    connect(&remoteProtocol, SIGNAL(peerListAdded(Peer)), this, SLOT(newUsers(Peer)), Qt::DirectConnection);
//    connect(&remoteProtocol, SIGNAL(simple()), this, SLOT(newUsers()), Qt::DirectConnection);
}

Example::~Example(){

}
void Example::newUsers(Peer peer){
    std::cout << "We have new user\n";
}

void Example::newUsers(){
    std::cout << "We have new user\n";
}

void Example::newClientConnection(){
    std::cout << "Server have new connection from client\n";
}

void Example::receiveTextComplete(QString newMessage){
    qDebug() << newMessage << "\n";
}

void Example::menu(){
    int choice;
    std::cout << "Hello in menu\n ";
    std::cout << "Input 1 for create Server\n";
    std::cout << "Input 2 for create Client\n";

    std::cin >> choice;

    if(choice == 1){
        std::cout << "You're choice is server\n";
        remoteProtocol.runTcpServer();
    }
    if(choice == 2){
        std::string ip;
        int port;

        std::cout << "You're choice is client\n";

        std::cout << "Please input ip or -1 if you'll want to use local 127.0.0.1 ip\n";
        std::cin >> ip;

        std::cout << "Please input port or -1 if you'll want to use standart 4644 port\n";
        std::cin >> port;

        createClient(ip, port);

        std::string message;
        std::cout << "Please input message for server\n";
        std::cin >> message;

        remoteProtocol.sendData(QString::fromUtf8(message.c_str()));
    }
}

void Example::createClient(std::string ip, int port){
    remoteProtocol.runUdpSocket();
    remoteProtocol.newOutcomingConnection(QString::fromUtf8(ip.c_str()), port);
}

void Example::createServer(){
    remoteProtocol.runTcpServer();
    remoteProtocol.runUdpSocket();

    connect(&remoteProtocol, SIGNAL(newClientConnection()), this, SLOT(newClientConnection()), Qt::DirectConnection);
    QObject::connect(&remoteProtocol, &RemoteProtocol::receiveTextComplete, this, &Example::receiveTextComplete);
//    receiveTextComplete
}

void Example::sayHello(){
    remoteProtocol.sayHello(QHostAddress::Broadcast, true);
}




