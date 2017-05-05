#include "connectionhandler.h"

#define INIT_KEY_SERVER 1
#define INIT_KEY_CLIENT 2
#define REQUEST_PASSWORD 3
#define GET_PASSWORD 4
#define CONSOLE_OUT 5
#define CONSOLE_IN 6

#define COUNT_QINT32 5

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
    this->socket.reset(socket.data());
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

        std::vector<char> temp;
        temp.resize(1);
        write(temp, REQUEST_PASSWORD);
    }
    else if(type == REQUEST_PASSWORD)
    {
        std::vector<char> temp;
        read(temp);

        std::string str;
        std::cin >> str;
        std::vector<char> passwordVector(str.begin(), str.end());
        write(passwordVector, GET_PASSWORD);
    }
    else if(type == GET_PASSWORD)
    {
        std::vector<char> passwordFromClient;
        read(passwordFromClient);

        if(!equal(password.begin(), password.end(), passwordFromClient.begin()))
        {
            closedConnection();
            return;
        }
        authorization = true;

        console.startServer(L"MYDESKTOP");
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
    QByteArray qbytearray;

    in >> size;
    qbytearray.resize(size);

    int readByte = in.readRawData(qbytearray.data(), size);
    if(readByte == -1)
        return -1;

    keyExchange.resize(size/sizeof(char));
    ConvertorData::qbytearray_to_data(qbytearray, &keyExchange[0], size);

    return 0;
}

// client side
int ConnectionHandler::write(DataIn& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray qbytearrayConsoleScreenBufferInfo;
    QByteArray qbytearrayInputRecords;

    qint32 sizeConsoleScreenBufferInfo = sizeof(data.consoleScreenBufferInfo);
    qint32 sizeInputRecords = (int)data.inputRecords.capacity() * sizeof(INPUT_RECORD);

    ConvertorData::data_to_qbytearray(&data.consoleScreenBufferInfo, qbytearrayConsoleScreenBufferInfo, sizeConsoleScreenBufferInfo);
    ConvertorData::data_to_qbytearray(&data.inputRecords[0], qbytearrayInputRecords, sizeInputRecords);

    qbytearrayConsoleScreenBufferInfo = aes.encrypt(qbytearrayConsoleScreenBufferInfo);
    qbytearrayInputRecords = aes.encrypt(qbytearrayInputRecords);

    qint32 allSize = qbytearrayConsoleScreenBufferInfo.size() + qbytearrayInputRecords.size();

    out << (quint32)CONSOLE_IN;
    out << allSize;

    out << qbytearrayConsoleScreenBufferInfo.size();
    out << qbytearrayInputRecords.size();

    if(out.writeRawData(qbytearrayConsoleScreenBufferInfo.data(), qbytearrayConsoleScreenBufferInfo.size()) == -1)
        return -1;
    if(out.writeRawData(qbytearrayInputRecords.data(), qbytearrayInputRecords.size()) == -1)
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
    qint32 sizeCharInfos;

    in >> sizeRect;
    in >> sizePosition;
    in >> sizeCharInfos;

    QByteArray qbytearrayRect = socket->read(sizeRect);
    QByteArray qbytearrayPosition = socket->read(sizePosition);
    QByteArray qbytearrayCharInfos = socket->read(sizeCharInfos);

    qbytearrayRect = aes.decrypt(qbytearrayRect);
    qbytearrayPosition = aes.decrypt(qbytearrayPosition);
    qbytearrayCharInfos = aes.decrypt(qbytearrayCharInfos);

    data.charInfos.resize(qbytearrayCharInfos.size()/sizeof(CHAR_INFO));

    ConvertorData::qbytearray_to_data(qbytearrayRect, &data.srctReadRect, qbytearrayRect.size());
    ConvertorData::qbytearray_to_data(qbytearrayPosition, &data.position, qbytearrayPosition.size());
    ConvertorData::qbytearray_to_data(qbytearrayCharInfos, &data.charInfos[0], qbytearrayCharInfos.size());

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

    qint32 sizeRect = sizeof(data.srctReadRect);
    qint32 sizePosition = sizeof(data.position);
    qint32 sizeCharInfos = (int)data.charInfos.capacity() * sizeof(CHAR_INFO);

    ConvertorData::data_to_qbytearray(&data.srctReadRect, qbytearrayRect, sizeRect);
    ConvertorData::data_to_qbytearray(&data.position, qbytearrayPosition, sizePosition);
    ConvertorData::data_to_qbytearray(&data.charInfos[0], qbytearrayCharInfos, sizeCharInfos);

    qbytearrayRect = aes.encrypt(qbytearrayRect);
    qbytearrayPosition = aes.encrypt(qbytearrayPosition);
    qbytearrayCharInfos = aes.encrypt(qbytearrayCharInfos);

    out << (quint32)0; // type
    out << (quint32)0; // all size

    out << (quint32)0; // srctReadRect size
    out << (quint32)0; // position size
    out << (quint32)0; // charInfos size

    block.append(qbytearrayRect);
    block.append(qbytearrayPosition);
    block.append(qbytearrayCharInfos);

    out.device()->seek(0);
    out << (quint32)CONSOLE_OUT;
    out << (quint32)(block.size() - sizeof(quint32)*COUNT_QINT32);

    out << (quint32)qbytearrayRect.size();
    out << (quint32)qbytearrayPosition.size();
    out << (quint32)qbytearrayCharInfos.size();

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

    qint32 sizeConsoleScreenBufferInfo;
    qint32 sizeInputRecords;

    in >> allSize;

    in >> sizeConsoleScreenBufferInfo;
    in >> sizeInputRecords;

    QByteArray qbytearrayConsoleScreenBufferInfo;
    QByteArray qbytearrayInputRecords;

    qbytearrayConsoleScreenBufferInfo.resize(sizeConsoleScreenBufferInfo);
    qbytearrayInputRecords.resize(sizeInputRecords);

    int readByteConsoleScreenBufferInfo = in.readRawData(qbytearrayConsoleScreenBufferInfo.data(), sizeConsoleScreenBufferInfo);
    if(readByteConsoleScreenBufferInfo == -1)
        return -1;
    int readByteInputRecords = in.readRawData(qbytearrayInputRecords.data(), sizeInputRecords);
    if(readByteInputRecords == -1)
        return -1;

    qbytearrayConsoleScreenBufferInfo = aes.decrypt(qbytearrayConsoleScreenBufferInfo);
    qbytearrayInputRecords = aes.decrypt(qbytearrayInputRecords);

    data.inputRecords.resize(qbytearrayInputRecords.size()/sizeof(INPUT_RECORD));

    ConvertorData::qbytearray_to_data(qbytearrayConsoleScreenBufferInfo, &data.consoleScreenBufferInfo, qbytearrayConsoleScreenBufferInfo.size());
    ConvertorData::qbytearray_to_data(qbytearrayInputRecords, &data.inputRecords[0], qbytearrayInputRecords.size());

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
