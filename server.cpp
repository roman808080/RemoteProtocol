#include "server.h"

Server::Server(QSharedPointer<QTcpSocket> socket)
{
    this->socket.reset(socket.data());
    connect(this->socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
    connect(this->socket.data(), SIGNAL(disconnected()),
            this, SLOT(closedConnection()));
    connect(this->socket.data(), SIGNAL(readyRead()),
            this, SLOT(exchange()));
}

Server::~Server()
{
    closedConnection();
}

int Server::write(DataOut& data)
{
//    QByteArray simple("I am Roman. I say you hello");
//    QByteArray second;
//    qDebug() << simple.size();
//    second.setRawData(simple, 3);
//    qDebug() << second;
//    qDebug() << simple;
//    simple.remove(0, 3);
//    qDebug() << simple;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    QByteArray qbytearray;
    qint32 size = sizeof(data);
    qint32 parts = ceil(size/200);
    qDebug() << "how many parts we must do! "<< parts;
    qDebug() << "server side. size struct: " << size;
    ConvertorData::data_to_qbytearray(&data, qbytearray, size);

    out << size;
    int writedBytes = out.writeRawData(qbytearray.data(), size);
    if(writedBytes == -1)
        return -1;
    qDebug() << "server side. write: " << writedBytes;

    socket->write(block);
    socket->waitForBytesWritten();
    return 0;
}

int Server::read(DataIn& data)
{
//    socket->waitForReadyRead();
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_4_0);

    qint32 size;
    in >> size;

    QByteArray dataArray;
    dataArray.resize(size);

    int readByte = in.readRawData(dataArray.data(), size);
    if(readByte == -1)
        return -1;

    qDebug() << "all " << size << " read " << readByte;

    ConvertorData::qbytearray_to_data(dataArray, &data, size);
    return 0;
}

//void Server::loop()
//{
//    QObject::connect(this, SIGNAL(finished()), this, SLOT(quit()));
//    start();
//    run();
//}

void Server::exchange()
{
    qDebug() << "hell";
//    ServerConsole serverConsole;
//    while(true){
    // first read
    DataIn nextMessage;
    read(nextMessage);
//        serverConsole.writeInputToConsole(nextMessage);

    qDebug() << nextMessage.array << "\n";
    // after write our console answer
    DataOut answer;
    int i = 0;
    qDebug() << "write our answer";
    for(char c: "Answer"){
        answer.array[i] = c;
        qDebug() << "from string " <<  c << "from array " << answer.array[i];
        i++;
    }
    write(answer);
//   }
}

void Server::sendConnectError(QAbstractSocket::SocketError e)
{
    if (socket)
    {
        socket->close();
        socket->deleteLater();
    }
}

void Server::closedConnection()
{
    if (socket)
       {
           socket->disconnect();
           socket->disconnectFromHost();
           socket->close();
           socket->deleteLater();
       }
}
