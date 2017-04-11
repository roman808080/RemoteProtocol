#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QDataStream>
#include <iostream>

#include "example.h"

Example::Example(){
    connect(&remoteProtocol, SIGNAL(peerListAdded(Peer)), this, SLOT(newUsers(Peer)), Qt::DirectConnection);
    QObject::connect(&remoteProtocol, &RemoteProtocol::newInConnection, this, &Example::newInConnection, Qt::DirectConnection);
    QObject::connect(&remoteProtocol, &RemoteProtocol::newOutConnection, this, &Example::newOutConnection, Qt::DirectConnection);
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
        std::cout << "Server have new connection from client\n";
        while(true){

            // first read
            socket->waitForReadyRead();
            QDataStream in;
            in.setDevice(socket.data());
            in.setVersion(QDataStream::Qt_4_0);

//            in.startTransaction();

            QString nextMessage;
            in >> nextMessage;

            qDebug() << nextMessage << "\n";
            // after write our console answer

            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);

            out << QString("Answer");
            socket->write(block);
            socket->waitForBytesWritten();
       }
}

void Example::newOutConnection(QSharedPointer<QTcpSocket> socket){
        std::cout << "Client have new connection with server\n";
        while(true){
            // first command
            std::cout << "Input text\n";
            std::string str;
            std::cin >> str;

            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);

            out << QString::fromStdString(str);

            socket->write(block);
            socket->waitForBytesWritten();

            // after read

            socket->waitForReadyRead();

            QDataStream in;
            in.setDevice(socket.data());
            in.setVersion(QDataStream::Qt_4_0);

//            in.startTransaction();

            QString nextMessage;
            in >> nextMessage;

            qDebug() << nextMessage << "\n";
        }
}

void Example::menu(){
    int choice;
//    while(true){
        std::cout << "Hello in menu\n ";
        std::cout << "Input 1 for create Server\n";
        std::cout << "Input 2 for create Client\n";
//        std::cout << "Input 3 for input message from Client to Server\n";
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
//        else if(choice == 3){
//            std::string message;
//            std::cout << "Please input message for server\n";
//            std::cin >> message;

////            remoteProtocol.sendData(QString::fromUtf8(message.c_str()));
//        }
//        else if(choice == -1){
//            break;
//        }
        else {
            std::cout << "Invalid comand\n";
        }
//    }

}

void Example::createClient(std::string ip, int port){
    if(ip == "-1")
        ip = "127.0.0.1";
    if(port == -1)
        port = 4644;

    remoteProtocol.runUdpSocket();
    remoteProtocol.newOutcomingConnection(QString::fromUtf8(ip.c_str()), port);
}

void Example::createServer(){
    remoteProtocol.runTcpServer();
    remoteProtocol.runUdpSocket();
//    connect(&remoteProtocol, SIGNAL(newClientConnection()), this, SLOT(newClientConnection()), Qt::DirectConnection);
//    QObject::connect(&remoteProtocol, &RemoteProtocol::receiveTextComplete, this, &Example::receiveTextComplete);
}

void Example::sayHello(){
    remoteProtocol.sayHello(QHostAddress::Broadcast, true);
}




