#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <iostream>

#include "example.h"



struct anyStruct
{
short sVal;
float fVal;
double dVal;
short Empty;
char array[8];
};

void operator <<(QDataStream &out, const anyStruct &any)
{
    out << any.sVal;
    out << any.fVal;
    out << any.dVal;
    out << any.Empty;
    std::vector<char>& str;
    data_to_vector(any, str, sizeof(any));
    out.writeRawData(any.array, sizeof(any.array));
}


Example::Example(){
//    connect(&remoteProtocol, SIGNAL(peerListAdded(Peer)), this, SLOT(newUsers(Peer)), Qt::DirectConnection);
    QObject::connect(&remoteProtocol, &TcpProtocol::newInConnection, this, &Example::newInConnection, Qt::DirectConnection);
    QObject::connect(&remoteProtocol, &TcpProtocol::newOutConnection, this, &Example::newOutConnection, Qt::DirectConnection);
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

 void Example::write(QSharedPointer<QTcpSocket> socket, char str[]){
     QByteArray block;
     QDataStream out(&block, QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_4_0);

     out << str;

//     out.device()->seek(0);
//     out << (quint16)(block.size() - sizeof(quint16));

     socket->write(block);
     socket->waitForBytesWritten();
 }

void Example::newInConnection(QSharedPointer<QTcpSocket> socket){
        std::cout << "Server have new connection from client\n";
        while(true){

            // first read
            socket->waitForReadyRead();
            QDataStream in;
            in.setDevice(socket.data());
            in.setVersion(QDataStream::Qt_4_0);

            char nextMessage[100];
//            in >> nextMessage;
            in.writeRawData("here",4);

            std::cout << nextMessage << "\n";
            // after write our console answer

            write(socket, "Answer");
       }
}

void Example::newOutConnection(QSharedPointer<QTcpSocket> socket){
        std::cout << "Client have new connection with server\n";
        while(true){
            // first command
            std::cout << "Input text\n";
            char str[100];
            std::cin >> str;
            write(socket, str);

            // after read

            socket->waitForReadyRead();

            QDataStream in;
            in.setDevice(socket.data());
            in.setVersion(QDataStream::Qt_4_0);

            std::string nextMessage;
//            in >> nextMessage;
            in.writeRawData("hello", 4);

            std::cout << nextMessage.c_str() << "\n";

        }
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

    remoteProtocol.newOutcomingConnection(QString::fromUtf8(ip.c_str()), port);
}

void Example::createServer(){
    remoteProtocol.runTcpServer();
//    connect(&remoteProtocol, SIGNAL(newClientConnection()), this, SLOT(newClientConnection()), Qt::DirectConnection);
//    QObject::connect(&remoteProtocol, &RemoteProtocol::receiveTextComplete, this, &Example::receiveTextComplete);
}

void Example::sayHello(){
//    remoteProtocol.sayHello(QHostAddress::Broadcast, true);
}


int vector_to_data(const std::vector<char>& str, void* data, int size)
{
    if (str.size() < size - 1)
        return -1;
    for (int i = 0; i < size; i++)
        ((char*)data)[i] = str[i];
    return 0;
}

int data_to_vector(void* data, std::vector<char>& str, int size)
{
    str.resize(size);
    for (int i = 0; i < size; i++)
        str[i] = ((char*)data)[i];
    return 0;
}
