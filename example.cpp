#include "example.h"

Example::Example(){
    QObject::connect(&remoteProtocol, &TcpProtocol::newInConnection, this, &Example::newInConnection);
}

Example::~Example(){}

 void Example::newInConnection(QSharedPointer<QTcpSocket> socket){
     std::cout << "Server have new connection from client\n";
     server.reset(new Server(socket));
     server->startExchange();
 }

 void Example::newOutConnection(QSharedPointer<QTcpSocket> socket){
     std::cout << "Client have new connection with server\n";
     client.reset(new Client(socket));
//     client->startExchange();
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
        ip = "127.0.0.1";
        port = 4644;
        createClient(ip, port);
    }
    else {
        std::cout << "Invalid comand\n";
    }
}

void Example::createClient(std::string ip, int port){
    QObject::connect(&remoteProtocol, &TcpProtocol::newOutConnection, this, &Example::newOutConnection, Qt::DirectConnection);
    remoteProtocol.newOutcomingConnection(QString::fromUtf8(ip.c_str()), port);
}

void Example::createServer(){
    remoteProtocol.runTcpServer();
}
