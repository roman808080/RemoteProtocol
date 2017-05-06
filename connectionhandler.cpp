#include "connectionhandler.h"

#define INIT_KEY_SERVER 1
#define INIT_KEY_CLIENT 2
#define REQUEST_PASSWORD 3
#define GET_PASSWORD 4
#define CONSOLE_OUT 5
#define CONSOLE_IN 6

#define COUNT_QINT32 6

ConnectionHandler::ConnectionHandler()
{
    readBufferSize = 0;
    type = 0;

    unsigned char buffer[MODULE_LENGTH] = {
                                               0x01, 0x45, 0x76, 0x87,
                                               0x99, 0x12, 0x11, 0x90,
                                               0x65, 0x65, 0x33, 0x17,
                                               0x82, 0x50, 0x71, 0x03
                                           };
    cryptoPModule = std::vector<char>((char*)buffer, (char*)buffer + MODULE_LENGTH);
    cryptoGModule = 0x02;
    authorization = false;

    aes.setMode(QTinyAes::ECB);
}

ConnectionHandler::ConnectionHandler(QSharedPointer<QTcpSocket> socket)
{
    setSocket(socket);

    readBufferSize = 0;
    type = 0;
}

ConnectionHandler::~ConnectionHandler()
{
    closedConnection();
}


void ConnectionHandler::startServer()
{
    exchanger.InitDiffieHellmanKeysExchanger(cryptoPModule, cryptoGModule);
    std::vector<char> exchangeKey;
    exchanger.GenerateExchangeData(exchangeKey);
    write(exchangeKey, INIT_KEY_SERVER);
}

void ConnectionHandler::setSocket(QSharedPointer<QTcpSocket> socket)
{
    this->socket = socket;
    connect(this->socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
    connect(this->socket.data(), SIGNAL(disconnected()),
            this, SLOT(closedConnection()));
    connect(this->socket.data(), SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void ConnectionHandler::setPassword(std::vector<char> password)
{
    this->password = password;
}

void ConnectionHandler::readyRead()
{
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_5_4);

    if(!type)
    {
        in >> type;
    }

    if(type == INIT_KEY_SERVER)
    {
        std::vector<char> keyExchange;
        read(keyExchange);

        exchanger.InitDiffieHellmanKeysExchanger(cryptoPModule, cryptoGModule);
        std::vector<char> exchangeKey;
        exchanger.GenerateExchangeData(exchangeKey);
        write(exchangeKey, INIT_KEY_CLIENT);

        exchanger.CompleteExchangeData(keyExchange, key);
        aes.setKey(QByteArray(&key[0], MODULE_LENGTH));
    }
    else if(type == INIT_KEY_CLIENT)
    {
        std::vector<char> keyExchange;
        read(keyExchange);

        exchanger.CompleteExchangeData(keyExchange, key);
        aes.setKey(QByteArray(&key[0], MODULE_LENGTH));

        write(REQUEST_PASSWORD);
    }
    else if(type == REQUEST_PASSWORD)
    {
        std::string str;
        std::cout << "Input password\n";
        std::cin >> str;
        std::vector<char> passwordVector(str.begin(), str.end());
        writePassword(passwordVector, GET_PASSWORD);
    }
    else if(type == GET_PASSWORD)
    {
        std::vector<char> passwordFromClient;
        readPassword(passwordFromClient);

        if(!equal(password.begin(), password.end(), passwordFromClient.begin()))
        {
            write(REQUEST_PASSWORD);

            readBufferSize = 0;
            type = 0;

            return;
        }
        authorization = true;

        console.startServer(/*L"MYDESKTOP"*/);
        DataOut dataOut;
        console.readOutputFromConsole(dataOut);
        write(dataOut);
    }
    else if(type == CONSOLE_OUT)// client side
    {
        // read output for console and write to console
        DataOut dataOut;
        if(read(dataOut) == -1)
            return;
        console.writeOutputToConsole(dataOut);

        // after read input for console and write to socket
        DataIn dataIn;
        console.readInputFromConsole(dataIn);
        write(dataIn);
    }
    else if(type == CONSOLE_IN)
    {
        if(!authorization)
            return;

        DataIn dataIn;
        read(dataIn);
        console.writeInputToConsole(dataIn);

        DataOut dataOut;
        console.readOutputFromConsole(dataOut);
        write(dataOut);
    }

    readBufferSize = 0;
    type = 0;
}

// exchange key
int ConnectionHandler::write(std::vector<char>& keyExchange, int code)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbytearray;
    qint32 size = (int)keyExchange.capacity() * sizeof(char);
    ConvertorData::data_to_qbytearray(&keyExchange[0], qbytearray, size);

    out << (quint32)code;
    out << size;

    if(out.writeRawData(qbytearray.data(), size) == -1)
        return -1;

    socket->write(block);
    socket->waitForBytesWritten();

    return 0;
}

// exchange key
int ConnectionHandler::read(std::vector<char>& keyExchange)
{
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_5_4);

    qint32 size;
    QByteArray qba;

    in >> size;
    qba.resize(size);

    int readByte = in.readRawData(qba.data(), size);
    if(readByte == -1)
        return -1;

    keyExchange.resize(size/sizeof(char));
    ConvertorData::qbytearray_to_data(qba, &keyExchange[0], size);

    return 0;
}

// write request
int ConnectionHandler::write(int code)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << (quint32)code;

    socket->write(block);
    socket->waitForBytesWritten();

    return 0;
}

// send password
int ConnectionHandler::writePassword(std::vector<char>& keyExchange, int code)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qba;
    qint32 size = (int)keyExchange.capacity() * sizeof(char);
    ConvertorData::data_to_qbytearray(&keyExchange[0], qba, size);

    qba = aes.encrypt(qba);

    out << (quint32)code;
    out << qba.size();

    if(out.writeRawData(qba.data(), qba.size()) == -1)
        return -1;

    socket->write(block);
    socket->waitForBytesWritten();

    return 0;
}

// read password
int ConnectionHandler::readPassword(std::vector<char>& keyExchange)
{
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_5_4);

    qint32 size;
    QByteArray qba;

    in >> size;
    qba.resize(size);

    int readByte = in.readRawData(qba.data(), size);
    if(readByte == -1)
        return -1;

    qba = aes.decrypt(qba);
    keyExchange.resize(qba.size()/sizeof(char));

    ConvertorData::qbytearray_to_data(qba, &keyExchange[0], qba.size());

    return 0;
}


// client side
int ConnectionHandler::write(DataIn& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbaAll;
    QByteArray qbaCsbi;
    QByteArray qbaInputRecords;

    qint32 sizeCsbi = sizeof(data.consoleScreenBufferInfo);
    qint32 sizeInputRecords = (int)data.inputRecords.capacity() * sizeof(INPUT_RECORD);

    ConvertorData::data_to_qbytearray(&data.consoleScreenBufferInfo, qbaCsbi, sizeCsbi);
    ConvertorData::data_to_qbytearray(&data.inputRecords[0], qbaInputRecords, sizeInputRecords);

    qbaAll.append(qbaCsbi);
    qbaAll.append(qbaInputRecords);

    qbaAll = aes.encrypt(qbaAll);

    qint32 allSize = qbaAll.size();//qbaCsbi.size() + qbaInputRecords.size();

    out << (quint32)CONSOLE_IN;
    out << allSize;

    out << qbaCsbi.size();
    out << qbaInputRecords.size();

    if(out.writeRawData(qbaAll.data(), qbaAll.size()) == -1)
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
        return -1;
    if(socket->bytesAvailable() < readBufferSize)
        return -1;

    qint32 sizeRect;
    qint32 sizePosition;
    qint32 sizeSize;
    qint32 sizeCharInfos;

    in >> sizeRect;
    in >> sizePosition;
    in >> sizeSize;
    in >> sizeCharInfos;

    QByteArray qbaAll =  socket->readAll();
    qbaAll = aes.decrypt(qbaAll);

    data.charInfos.resize(sizeCharInfos/sizeof(CHAR_INFO));

    int pos = 0;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizeRect), &data.srctReadRect, sizeRect);
    pos += sizeRect;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizePosition), &data.position, sizePosition);
    pos += sizePosition;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizeSize), &data.size, sizeSize);
    pos += sizeSize;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizeCharInfos), &data.charInfos[0], sizeCharInfos);
    pos += sizeCharInfos;

    return 0;
}

// server side
int ConnectionHandler::write(DataOut& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbaAll;
    QByteArray arrayQba[COUNT_QINT32-2];

    qint32 sizeRect = sizeof(data.srctReadRect);
    qint32 sizePosition = sizeof(data.position);
    qint32 sizeSize = sizeof(data.size);
    qint32 sizeCharInfos = (int)data.charInfos.capacity() * sizeof(CHAR_INFO);


    ConvertorData::data_to_qbytearray(&data.srctReadRect, arrayQba[0], sizeRect);
    ConvertorData::data_to_qbytearray(&data.position, arrayQba[1], sizePosition);
    ConvertorData::data_to_qbytearray(&data.size, arrayQba[2], sizeSize);
    ConvertorData::data_to_qbytearray(&data.charInfos[0], arrayQba[3], sizeCharInfos);
    for(int i=0; i<COUNT_QINT32-2; i++)
    {
        qbaAll.append(arrayQba[i]);
    }

    qbaAll = aes.encrypt(qbaAll);

    out << (quint32)0; // type
    out << (quint32)0; // all size

    out << (quint32)0; // srctReadRect size
    out << (quint32)0; // position size
    out << (quint32)0; // size
    out << (quint32)0; // charInfos size

    block.append(qbaAll);

    out.device()->seek(0);
    out << (quint32)CONSOLE_OUT;
    out << (quint32)(block.size() - sizeof(quint32)*COUNT_QINT32);

    out << (quint32)sizeRect;
    out << (quint32)sizePosition;
    out << (quint32)sizeSize;
    out << (quint32)sizeCharInfos;

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
    qint32 sizeCsbi;
    qint32 sizeInputRecords;

    in >> allSize;
    in >> sizeCsbi;
    in >> sizeInputRecords;

    QByteArray qbaAll;
    qbaAll.resize(allSize);
    int readBytes = in.readRawData(qbaAll.data(), allSize);
    if(readBytes == -1)
        return -1;

    qbaAll = aes.decrypt(qbaAll);

    data.inputRecords.resize(sizeInputRecords/sizeof(INPUT_RECORD));

    int pos = 0;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizeCsbi), &data.consoleScreenBufferInfo, sizeCsbi);
    pos += sizeCsbi;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizeInputRecords), &data.inputRecords[0], sizeInputRecords);
    pos += sizeInputRecords;

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
