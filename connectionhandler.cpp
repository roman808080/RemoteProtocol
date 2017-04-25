#include "connectionhandler.h"

#define CONSOLE_OUT 1
#define CONSOLE_IN 2

#define COUNT_QINT32 6

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
//    console.readOutputFromConsole(dataOut);

    std::string str = "hello client\n";
    dataOut.example.resize(str.size() + 1);
    int i = 0;
    for(auto c : str)
    {
        dataOut.example[i] = c;
        i++;
    }

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

        for(auto c: dataOut.example)
        {
            std::cout << c;
        }
        std::cout << std::endl;
//        console.writeOutputToConsole(dataOut);

        // after read input for console and write to socket
        DataIn dataIn;
//        console.readInputFromConsole(dataIn);

        std::string str = "hello server\n";
        dataIn.example.resize(str.size() + 1);
        int i = 0;
        for(auto c : str)
        {
            dataIn.example[i] = c;
            i++;
        }

        write(dataIn);

        Sleep(3000);
    }
    else if(type == CONSOLE_IN)
    {
        DataIn dataIn;
        read(dataIn);

        for(auto c: dataIn.example)
        {
            std::cout << c;
        }
        std::cout << std::endl;

//        console.writeInputToConsole(dataIn);

        DataOut dataOut;
//        console.readOutputFromConsole(dataOut);

        std::string str = "hello client\n";
        dataOut.example.resize(str.size() + 1);
        int i = 0;
        for(auto c : str)
        {
            dataOut.example[i] = c;
            i++;
        }

        write(dataOut);

        Sleep(3000);
    }
    else
    {
        throw std::runtime_error("Not correct type input or output for console");
    }
}

// client side
int ConnectionHandler::write(DataIn& data)
{
//    SMALL_RECT srctWriteRect = {0};
//    std::vector<INPUT_RECORD> inputRecords;
//    std::vector<char> example;

//    qint32 sizeRect = sizeof(data.srctReadRect);
//    qint32 sizePosition = sizeof(data.position);
//    qint32 sizeCharInfos = (int)data.charInfos.capacity() * sizeof(CHAR_INFO);
//    qint32 sizeExample = (int)data.example.capacity() * sizeof(char);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbytearrayRect;
    QByteArray qbytearrayInputRecords;
    QByteArray qbytearrayExample;

    qint32 sizeRect = sizeof(data.srctWriteRect);
    qint32 sizeInputRecords = (int)data.inputRecords.capacity() * sizeof(INPUT_RECORD);
    qint32 sizeExample = (int)data.example.capacity() * sizeof(char);

    qint32 allSize = sizeRect + sizeInputRecords + sizeExample;

    ConvertorData::data_to_qbytearray(&data.srctWriteRect, qbytearrayRect, sizeRect);
    ConvertorData::data_to_qbytearray(&data.inputRecords[0], qbytearrayInputRecords, sizeInputRecords);
    ConvertorData::data_to_qbytearray(&data.example[0], qbytearrayExample, sizeExample);

    out << (quint32)CONSOLE_IN;
    out << allSize;

    out << sizeRect;
    out << sizeInputRecords;
    out << sizeExample;

    if(out.writeRawData(qbytearrayRect.data(), sizeRect) == -1)
        return -1;
    if(out.writeRawData(qbytearrayInputRecords.data(), sizeInputRecords) == -1)
        return -1;
    if(out.writeRawData(qbytearrayExample.data(), sizeExample) == -1)
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

//    QByteArray dataArray = socket->readAll();
    qint32 sizeRect;
    qint32 sizePosition;
    qint32 sizeCharInfos;
    qint32 sizeExample;

    in >> sizeRect;
    in >> sizePosition;
    in >> sizeCharInfos;
    in >> sizeExample;

    QByteArray qbytearrayRect = socket->read(sizeRect);
    QByteArray qbytearrayPosition = socket->read(sizePosition);
    QByteArray qbytearrayCharInfos = socket->read(sizeCharInfos);
    QByteArray qbytearrayExample = socket->read(sizeExample);

//    qDebug() << "readBufferSize = " << readBufferSize << " actual size " << dataArray.size();

    data.charInfos.resize(sizeCharInfos/sizeof(CHAR_INFO));
    data.example.resize(sizeExample/sizeof(char));

    ConvertorData::qbytearray_to_data(qbytearrayRect, &data.srctReadRect, readBufferSize);
    ConvertorData::qbytearray_to_data(qbytearrayPosition, &data.position, readBufferSize);
    ConvertorData::qbytearray_to_data(qbytearrayCharInfos, &data.charInfos[0], readBufferSize);
    ConvertorData::qbytearray_to_data(qbytearrayExample, &data.example[0], readBufferSize);

    return 0;
}

// server side
int ConnectionHandler::write(DataOut& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbytearrayRect;
    QByteArray qbytearrayPosition;
    QByteArray qbytearrayCharInfos;
    QByteArray qbytearrayExample;

    qint32 sizeRect = sizeof(data.srctReadRect);
    qint32 sizePosition = sizeof(data.position);
    qint32 sizeCharInfos = (int)data.charInfos.capacity() * sizeof(CHAR_INFO);
    qint32 sizeExample = (int)data.example.capacity() * sizeof(char);

    ConvertorData::data_to_qbytearray(&data.srctReadRect, qbytearrayRect, sizeRect);
    ConvertorData::data_to_qbytearray(&data.position, qbytearrayPosition, sizePosition);
    ConvertorData::data_to_qbytearray(&data.charInfos[0], qbytearrayCharInfos, sizeCharInfos);
    ConvertorData::data_to_qbytearray(&data.example[0], qbytearrayExample, sizeExample);

    out << (quint32)0; // type
    out << (quint32)0; // all size

    out << (quint32)0; // srctReadRect size
    out << (quint32)0; // position size
    out << (quint32)0; // charInfos size
    out << (quint32)0; // example size

//    SMALL_RECT srctReadRect = {0};
//    COORD position = {0};
//    std::vector<CHAR_INFO> charInfos;
//    std::vector<char> example;

    block.append(qbytearrayRect);
    block.append(qbytearrayPosition);
    block.append(qbytearrayCharInfos);
    block.append(qbytearrayExample);

    out.device()->seek(0);
    out << (quint32)CONSOLE_OUT;
    out << (quint32)(block.size() - sizeof(quint32)*COUNT_QINT32);
    qDebug() << "server side write size = " << block.size() - sizeof(quint32)*COUNT_QINT32;

    out << (quint32)sizeRect;
    out << (quint32)sizePosition;
    out << (quint32)sizeCharInfos;
    out << (quint32)sizeExample;

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

    qint32 allSize;

    qint32 sizeRect;
    qint32 sizeInputRecords;
    qint32 sizeExample;

    in >> allSize;

    in >> sizeRect;
    in >> sizeInputRecords;
    in >> sizeExample;

    QByteArray qbytearrayRect;
    QByteArray qbytearrayInputRecords;
    QByteArray qbytearrayExample;

    qbytearrayRect.resize(sizeRect);
    qbytearrayInputRecords.resize(sizeInputRecords);
    qbytearrayExample.resize(sizeExample);

    int readByte;

    readByte = in.readRawData(qbytearrayRect.data(), sizeRect);
    if(readByte == -1)
        return -1;
    readByte = in.readRawData(qbytearrayInputRecords.data(), sizeInputRecords);
    if(readByte == -1)
        return -1;
    readByte = in.readRawData(qbytearrayExample.data(), sizeExample);
    if(readByte == -1)
        return -1;

    data.inputRecords.resize(sizeInputRecords/sizeof(INPUT_RECORD));
    data.example.resize(sizeExample/sizeof(char));

    ConvertorData::qbytearray_to_data(qbytearrayRect, &data.srctWriteRect, sizeRect);
    ConvertorData::qbytearray_to_data(qbytearrayInputRecords, &data.inputRecords[0], sizeInputRecords);
    ConvertorData::qbytearray_to_data(qbytearrayExample, &data.example[0], sizeExample);

    return 0;
}




//    SMALL_RECT srctWriteRect = {0};
//    std::vector<INPUT_RECORD> inputRecords;
//    std::vector<char> example;

//    qint32 sizeRect = sizeof(data.srctReadRect);
//    qint32 sizePosition = sizeof(data.position);
//    qint32 sizeCharInfos = (int)data.charInfos.capacity() * sizeof(CHAR_INFO);
//    qint32 sizeExample = (int)data.example.capacity() * sizeof(char);

//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_5_4);

//    QByteArray qbytearrayRect;
//    QByteArray qbytearrayInputRecords;
//    QByteArray qbytearrayExample;

//    qint32 sizeRect = sizeof(data.srctReadRect);
//    qint32 sizeInputRecords = (int)data.inputRecords.capacity() * sizeof(INPUT_RECORD);
//    qint32 sizeExample = (int)data.example.capacity() * sizeof(char);

//    qint32 allSize = sizeRect + sizeInputRecords + sizeExample;

//    ConvertorData::data_to_qbytearray(&data.srctWriteRect, qbytearrayRect, sizeRect);
//    ConvertorData::data_to_qbytearray(&data.inputRecords[0], qbytearrayInputRecords, sizeInputRecords);
//    ConvertorData::data_to_qbytearray(&data.example[0], qbytearrayExample, sizeExample);

//    out << (quint32)CONSOLE_IN;
//    out << allSize;

//    out << sizeRect;
//    out << sizeInputRecords;
//    out << sizeExample;

//    if(out.writeRawData(qbytearrayRect.data(), sizeRect) == -1)
//        return -1;
//    if(out.writeRawData(qbytearrayInputRecords.data(), sizeInputRecords) == -1)
//        return -1;
//    if(out.writeRawData(qbytearrayExample.data(), sizeExample) == -1)
//        return -1;

//    socket->write(block);
//    socket->waitForBytesWritten();




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
