#include "server.h"

#define SIZE_CHUNK 500

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
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbytearray;
    qint32 size = sizeof(data);

    ConvertorData::data_to_qbytearray(&data, qbytearray, size);

    out << (quint32)0;
    block.append(qbytearray);

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    qint64 x = 0;
    while (x < block.size()) {
        qint64 y = socket->write(block);
        x += y;
    }
    return 0;
}

int Server::read(DataIn& data)
{
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

    ConvertorData::qbytearray_to_data(dataArray, &data, size);
    return 0;
}


void Server::exchange()
{
    DataIn nextMessage;
    read(nextMessage);
    serverConsole.writeInputToConsole(nextMessage);

    // after write our console answer
    DataOut answer;
    write(answer);
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
