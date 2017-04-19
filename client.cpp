#include <QTcpSocket>
#include <QSharedPointer>
#include <QDataStream>
#include <iostream>
#include <QDebug>

#include "client.h"
#include "convertordata.h"
#include "datastruct.h"

Client::Client(QSharedPointer<QTcpSocket> socket)
{
    this->socket.reset(socket.data());
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

    ConvertorData::qbytearray_to_data(dataArray, &data, size);
    return 0;
}

void Client::loop()
{
    QObject::connect(this, SIGNAL(finished()), this, SLOT(quit()));
    start();
}

void Client::run()
{
    while(true){
        // first command
        std::cout << "Input text\n";
        DataIn data;
        std::cin >> data.array;
        write(data);

        // after read
        DataOut nextMessage;
        read(nextMessage);
        qDebug() << nextMessage.array << "\n";
    }
}
