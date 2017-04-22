#include "client.h"

Client::Client(QSharedPointer<QTcpSocket> socket)
{
    this->socket.reset(socket.data());
    connect(this->socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
    connect(this->socket.data(), SIGNAL(disconnected()),
            this, SLOT(closedConnection()));

    connect(this->socket.data(), SIGNAL(readyRead()), this, SLOT(endExchange()));
}

Client::~Client()
{
    closedConnection();
}

int Client::write(DataIn& data)
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

int Client::read(DataOut& data)
{
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_5_4);
    qDebug() << "i was here";

    if(!size && socket->bytesAvailable() >= sizeof(quint32))
        in >> size;
    if(!size)
        return 0;
    if(socket->bytesAvailable() < size)
        return 0;

    QByteArray dataArray = socket->readAll();
    qDebug() << "we read " << dataArray.size() << " we need " << size;

    ConvertorData::qbytearray_to_data(dataArray, &data, size);
    qDebug() << data.array << "here2\n";

    startExchange();

    return 0;
}

void Client::startExchange()
{
    size = 0;
    std::cout << "Input text\n";
    DataIn data;
    std::cin >> data.array;
    write(data);
}

void Client::endExchange()
{
    DataOut data;
    read(data);
}


void Client::sendConnectError(QAbstractSocket::SocketError e)
{
    if (socket)
    {
        socket->close();
        socket->deleteLater();
    }
}

void Client::closedConnection()
{
    if (socket)
       {
           socket->disconnect();
           socket->disconnectFromHost();
           socket->close();
           socket->deleteLater();
       }
}
