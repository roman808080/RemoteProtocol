#include <QTcpSocket>
#include <QSharedPointer>
#include <QDataStream>
#include <iostream>
#include <QDebug>

#include "client.h"
#include "convertordata.h"
#include "datastruct.h"
#include "inputconsole.h"

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
      qDebug()<<"From worker thread: "<< QThread::currentThreadId();
//    QObject::connect(this, SIGNAL(finished()), this, SLOT(quit()));
//    start();
    run();
}

void Client::run()
{
    while(true){
        // first command
        std::cout << "Input text\n";
        DataIn data;
        std::cin >> data.array;
        readInputFromConsole(data);
        write(data);

        // after read
        DataOut nextMessage;
        read(nextMessage);
        qDebug() << nextMessage.array << "here\n";
    }
}



int Client::readInputFromConsole(DataIn& data)
{
    //temporary
    std::string str = "cd ..";
//    data.inputRecords = wchars2records(str);
    INPUT_RECORD* ir = wchars2records(str);
    for(int i=0; i<str.size()+1; i++)
    {
        data.inputRecords[i] = ir[i];
    }
}

int Client::writeOutputToConsole(DataOut& data)
{
    //pass
}
