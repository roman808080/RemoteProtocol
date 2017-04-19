#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <iostream>
#include <windows.h>

#include "example.h"
#include "convertordata.h"


struct DataOut
{
    char array[8];
};

struct DataIn
{
    char array[8];
};

Example::Example(){
//    connect(&remoteProtocol, SIGNAL(peerListAdded(Peer)), this, SLOT(newUsers(Peer)), Qt::DirectConnection);
    QObject::connect(&remoteProtocol, &TcpProtocol::newInConnection, this, &Example::newInConnection, Qt::DirectConnection);
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

int Example::write(QSharedPointer<QTcpSocket> socket, DataOut& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    QByteArray qbytearray;
    qint32 size = sizeof(data);
    ConvertorData::data_to_qbytearray(&data, qbytearray, size);

    out << size;
    if(out.writeRawData(qbytearray.data(), size) == -1)
        return -1;

    socket->write(block);
    socket->waitForBytesWritten();
    return 0;
}

int Example::read(QSharedPointer<QTcpSocket> socket, DataOut& data)
{
    socket->waitForReadyRead();
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_4_0);

    qint32 size;
    in >> size;

    QByteArray dataArray;
    dataArray.resize(size);

    if(in.readRawData(dataArray.data(), size) == -1)
        return -1;

//     DataIn data;
    ConvertorData::qbytearray_to_data(dataArray, &data, size);
    return 0;
}

 int Example::write(QSharedPointer<QTcpSocket> socket, DataIn& data)
 {
     QByteArray block;
     QDataStream out(&block, QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_4_0);

     QByteArray qbytearray;
     qint32 size = sizeof(data);
     ConvertorData::data_to_qbytearray(&data, qbytearray, size);

     out << size;
     if(out.writeRawData(qbytearray.data(), size) == -1)
         return -1;

     socket->write(block);
     socket->waitForBytesWritten();
     return 0;
 }

 int Example::read(QSharedPointer<QTcpSocket> socket, DataIn& data)
 {
     socket->waitForReadyRead();
     QDataStream in;
     in.setDevice(socket.data());
     in.setVersion(QDataStream::Qt_4_0);

     qint32 size;
     in >> size;

     QByteArray dataArray;
     dataArray.resize(size);

     if(in.readRawData(dataArray.data(), size) == -1)
         return -1;

//     DataIn data;
     ConvertorData::qbytearray_to_data(dataArray, &data, size);
     return 0;
 }

 void Example::newInConnection(QSharedPointer<QTcpSocket> socket){
     connect(socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
     std::cout << "Server have new connection from client\n";
     while(true){

         // first read
         DataIn nextMessage;
         read(socket, nextMessage);

         qDebug() << nextMessage.array << "\n";
         // after write our console answer
         DataIn answer;
//             answer.array = "Answer";//{'A', 'n', 's', 'w', 'e', 'r', 0, 0};
         int i = 0;
         for(char c: "Answer"){
             answer.array[i] = c;
             i++;
         }
         write(socket, answer);
    }
 }

 void Example::newOutConnection(QSharedPointer<QTcpSocket> socket){
     std::cout << "Client have new connection with server\n";
     while(true){
         // first command
         std::cout << "Input text\n";
         DataIn data;
         std::cin >> data.array;
         write(socket, data);

         // after read
         DataIn nextMessage;
         read(socket, nextMessage);
         qDebug() << nextMessage.array << "\n";
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


//int qbytearray_to_data(const QByteArray& str, void* data, qint32 size)
//{
//    if (str.size() < size - 1)
//        return -1;
//    for (int i = 0; i < size; i++)
//        ((char*)data)[i] = str[i];
//    return 0;
//}

//int data_to_qbytearray(void* data, QByteArray& str, qint32 size)
//{
//    str.resize(size);
//    for (int i = 0; i < size; i++)
//        str[i] = ((char*)data)[i];
//    return 0;
//}


void Example::sendConnectError(QAbstractSocket::SocketError e)
{
//    if (mCurrentSocket)
//    {
//        mCurrentSocket->close();
//        mCurrentSocket->deleteLater();
//    }
}
