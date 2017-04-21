#include "client.h"

Client::Client(QSharedPointer<QTcpSocket> socket)
{
    this->socket.reset(socket.data());
    connect(this->socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
    connect(this->socket.data(), SIGNAL(disconnected()),
            this, SLOT(closedConnection()));
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
    socket->waitForReadyRead();
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_4_0);

    qint32 size;
    in >> size;

    QByteArray dataArray;
    dataArray.resize(size);
    qDebug() << "it's side of client. size = " << size;

    int readByte = in.readRawData(dataArray.data(), size);
    if( readByte == -1)
        return -1;
    qDebug() << "all " << size << " read " << readByte;
//    qint32 completed = 0;
//    while(completed < size)
//    while(socket->bytesAvailable())
//    {
//        //socket->bytesAvailable();
//        QByteArray tempDataArray;
//        tempDataArray.resize(size);

//        int readByte = in.readRawData(tempDataArray.data(), size);
//        if( readByte == -1)
//        {
//            qDebug() << "Have some error";
//            return -1;
//        }
//        dataArray.append(tempDataArray);
//        completed += readByte;
//        qDebug() << "we read data " << completed << " in this iteration " << readByte;
//    }

//    qDebug() << "we read " << completed;

    ConvertorData::qbytearray_to_data(dataArray, &data, size);
    qDebug() << data.array << "here2\n";
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
//    ClientConsole clientConsole;
    while(true){
        // first command
        std::cout << "Input text\n";
        DataIn data;
        std::cin >> data.array;
//        clientConsole.readInputFromConsole(data);
//        readInputFromConsole(data);
        write(data);

        // after read
        DataOut nextMessage;
        read(nextMessage);
        qDebug() << nextMessage.array << "here\n";
        for(char c : nextMessage.array){
            qDebug() << c;
        }
    }
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


//int Client::readInputFromConsole(DataIn& data)
//{
//    //temporary
//    std::string str = "cd ..";
////    data.inputRecords = wchars2records(str);
//    INPUT_RECORD* ir = wchars2records(str);
//    for(int i=0; i<str.size()+1; i++)
//    {
//        data.inputRecords[i] = ir[i];
//    }
//}

//int Client::writeOutputToConsole(DataOut& data)
//{
//    //pass
//}
