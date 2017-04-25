#include "example.h"

Example::Example(){
    QObject::connect(&remoteProtocol, &TcpProtocol::newInConnection, this, &Example::newInConnection);
}

Example::~Example(){}

 void Example::newInConnection(QSharedPointer<QTcpSocket> socket){
     std::cout << "Server have new connection from client\n";
     connectionHandler.reset(new ConnectionHandler(socket));
     connectionHandler->startServer();
 }

 void Example::newOutConnection(QSharedPointer<QTcpSocket> socket){
     std::cout << "Client have new connection with server\n";
     connectionHandler.reset(new ConnectionHandler(socket));
 }

 void just_do(char d[])
 {
     qDebug() << "in function " << sizeof(d)/sizeof(d[0]);
     qDebug() << "in function " << sizeof(d);

     for(int i = 0; i<4; i++)
     {
         std::cout << d[i];
     }
     std::cout << std::endl;
 }

 struct ert
 {
     int first;
     char second;
     double third;
 };

 void just_do2(ert t[], std::string str)
 {

 }

void Example::menu(){

    DataOut dataOut;
    qDebug() << sizeof(dataOut);
    std::string str = "hello client\n";
    dataOut.example.resize(str.size() + 1);
    qDebug() << "sizeof example vector" << sizeof(dataOut.example);
    qDebug() << "sizeof charInfos vector" << sizeof(dataOut.charInfos);
    qDebug() << "sizeof example capacity" << dataOut.example.capacity();
    qDebug() << "sizeof example capacity*char" << (int)dataOut.example.capacity() * sizeof(char);
    qDebug() << "sizeof(char) " << sizeof(char);
    int i = 0;
    for(auto c : str)
    {
        dataOut.example[i] = c;
        i++;
    }
    qDebug() << "sizeof example capacity*char" << dataOut.example.capacity() * sizeof(char);
//    qDebug() << "sizeof example capacity*char" << sizeof(dataOut.example.size());
    qDebug() << sizeof(dataOut);
    for(auto c: dataOut.example)
    {
        std::cout << c;
    }
    std::cout << std::endl;
    just_do(&dataOut.example[0]);

    QByteArray qbytearray;
    qint32 size = dataOut.example.capacity() * sizeof(char);
    ConvertorData::data_to_qbytearray(&dataOut.example[0], qbytearray, size);

    std::vector<char> other;
    other.resize(size);

    ConvertorData::qbytearray_to_data(qbytearray, &other[0], size);

    std::cout << "here\n";
    for(auto c: other)
    {
        std::cout << c;
    }
    std::cout << std::endl;


    std::vector<ert> qwe;
    qwe.resize(str.size());
    i = 0;
    std::cout << 85 << std::endl;
    for(char c: str)
    {
        qwe[i].first = i;
        qwe[i].second = c;
        qwe[i].third = 3.3;
        std::cout << qwe[i].second;
        std::cout << c;
        i++;
    }
    std::cout << std::endl;

    for(int i=0; i<qwe.size(); i++)
    {
//        std::cout << element.first << element.second << element.third;
        ert temp = qwe[i];
        std::cout << temp.second;
    }
    std::cout << std::endl;

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
