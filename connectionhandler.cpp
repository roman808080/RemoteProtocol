#include "connectionhandler.h"

#define CONSOLE_OUT 1
#define CONSOLE_IN 2

ConnectionHandler::ConnectionHandler(QSharedPointer<QTcpSocket> socket)
{
    this->socket.reset(socket.data());
    connect(this->socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
    connect(this->socket.data(), SIGNAL(disconnected()),
            this, SLOT(closedConnection()));
    connect(this->socket.data(), SIGNAL(readyRead()), this, SLOT(readyRead()));

    readBufferSize = 0;
}

ConnectionHandler::~ConnectionHandler()
{
    closedConnection();
}


void ConnectionHandler::startServer()
{
    DataOut dataOut;
    console.readOutputFromConsole(dataOut);
    write(dataOut);
}

void ConnectionHandler::readyRead()
{
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_5_4);

    qint32 type;
    in >> type;

    if(type == CONSOLE_OUT)
    {
        // read output for console and write to console
        readBufferSize = 0;
        DataOut dataOut;
        read(dataOut);
        console.writeOutputToConsole(dataOut);

        // after read input for console and write to socket
        DataIn dataIn;
        console.readInputFromConsole(dataIn);
        write(dataIn);
    }
    else if(type == CONSOLE_IN)
    {
        DataIn dataIn;
        read(dataIn);
        console.writeInputToConsole(dataIn);

        DataOut dataOut;
        console.readOutputFromConsole(dataOut);
        write(dataOut);
    }
    else
    {
        throw std::runtime_error("Not correct type input or output for console");
    }
}

// client side
int ConnectionHandler::write(DataIn& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbytearray;
    qint32 size = sizeof(data);
    ConvertorData::data_to_qbytearray(&data, qbytearray, size);

    out << (quint32)CONSOLE_IN;
    out << size;
    if(out.writeRawData(qbytearray.data(), size) == -1)
        return -1;

    socket->write(block);
    socket->waitForBytesWritten();
    return 0;
}

// client side
int ConnectionHandler::read(DataOut& data)
{
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_5_4);

    if(!readBufferSize && socket->bytesAvailable() >= sizeof(quint32))
        in >> readBufferSize;
    if(!readBufferSize)
        return 0;
    if(socket->bytesAvailable() < readBufferSize)
        return 0;

    QByteArray dataArray = socket->readAll();

    ConvertorData::qbytearray_to_data(dataArray, &data, readBufferSize);
    return 0;
}

// server side
int ConnectionHandler::write(DataOut& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbytearray;
    qint32 size = sizeof(data);

    ConvertorData::data_to_qbytearray(&data, qbytearray, size);

    out << (quint32)0;
    out << (quint32)0;
    block.append(qbytearray);

    out.device()->seek(0);
    out << (quint32)CONSOLE_OUT;
    out << (quint32)(block.size() - sizeof(quint32) - sizeof(quint32));

    qint64 x = 0;
    while (x < block.size())
    {
        qint64 y = socket->write(block);
        x += y;
    }
    return 0;
}

// server side
int ConnectionHandler::read(DataIn& data)
{
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_5_4);

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

void ConnectionHandler::sendConnectError(QAbstractSocket::SocketError e)
{
    if (socket)
    {
        socket->close();
        socket->deleteLater();
    }
}

void ConnectionHandler::closedConnection()
{
    if (socket)
       {
           socket->disconnect();
           socket->disconnectFromHost();
           socket->close();
           socket->deleteLater();
       }
}
