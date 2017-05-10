#include "connectionhandler.h"

#define INIT_KEY_SERVER 1
#define INIT_KEY_CLIENT 2
#define REQUEST_PASSWORD 3
#define GET_PASSWORD 4
#define CONSOLE_OUT 5
#define CONSOLE_IN 6
#define CONTROL_CONNECTION 7

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
    aliveState = true;
    controlConnection = false;
    server = false;

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
    server = true;
    exchanger.InitDiffieHellmanKeysExchanger(cryptoPModule, cryptoGModule);
    std::vector<char> exchangeKey;
    exchanger.GenerateExchangeData(exchangeKey);
    write(exchangeKey, INIT_KEY_SERVER);
}

bool ConnectionHandler::alive()
{
    return aliveState;
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

void ConnectionHandler::setSocketDescriptor(qintptr descriptor)
{
    socket.reset(new QTcpSocket);
    socket->setSocketDescriptor(descriptor);

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

void ConnectionHandler::setControlConnection(bool state)
{
    controlConnection = state;
}

void ConnectionHandler::readyRead()
{
    try
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
            qDebug() << "send key";
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
            if(controlConnection)
            {
                writePassword(password, CONTROL_CONNECTION);
            }
            else
            {
                std::string str;
                std::cout << "Input password\n";
                std::cin >> str;
                std::vector<char> passwordVector(str.begin(), str.end());
                writePassword(passwordVector, GET_PASSWORD);
            }
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

            console.startServer(L"NEWDESKTOP");
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
        else if(type == CONTROL_CONNECTION)
        {
            std::vector<char> passwordFromClient;
            readPassword(passwordFromClient);

            if(!equal(password.begin(), password.end(), passwordFromClient.begin()))
            {
                qDebug() << "controlConnection. Problem with password";
                write(REQUEST_PASSWORD);

                readBufferSize = 0;
                type = 0;

                return;
            }
            controlConnection = true;
            qDebug() << "controlConnection " << controlConnection;
        }

        readBufferSize = 0;
        type = 0;
    }
    catch(...)
    {
        closedConnection();
    }
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
    int count = 2;
    QByteArray arrayQba[count];

    qint32 sizeSt = sizeof(data.st);
    qint32 sizeInputRecords = (int)data.inputRecords.capacity() * sizeof(INPUT_RECORD);

    ConvertorData::data_to_qbytearray(&data.st, arrayQba[0], sizeSt);
    ConvertorData::data_to_qbytearray(&data.inputRecords[0], arrayQba[1], sizeInputRecords);

    for(int i=0; i<count; i++)
        qbaAll.append(arrayQba[i]);
    qbaAll = aes.encrypt(qbaAll);

    out << (quint32)CONSOLE_IN;
    out << (quint32)qbaAll.size();

    out << (quint32)sizeSt; // st size
    out << (quint32)sizeInputRecords; // inputRecords size

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

    qint32 sizeSt;
    qint32 sizeCharInfos;

    in >> sizeSt;
    in >> sizeCharInfos;

    QByteArray qbaAll =  socket->readAll();
    qbaAll = aes.decrypt(qbaAll);

    data.charInfos.resize(sizeCharInfos/sizeof(CHAR_INFO));

    int pos = 0;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizeSt), &data.st, sizeSt);
    pos += sizeSt;
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
    int count = 2;
    QByteArray arrayQba[count];

    qint32 sizeSt = sizeof(data.st);
    qint32 sizeCharInfos = (int)data.charInfos.capacity() * sizeof(CHAR_INFO);

    ConvertorData::data_to_qbytearray(&data.st, arrayQba[0], sizeSt);
    ConvertorData::data_to_qbytearray(&data.charInfos[0], arrayQba[1], sizeCharInfos);
    for(int i=0; i<count; i++)
        qbaAll.append(arrayQba[i]);
    qbaAll = aes.encrypt(qbaAll);

    out << (quint32)CONSOLE_OUT; // type
    out << (quint32)qbaAll.size(); // all size

    out << (quint32)sizeSt; // st size
    out << (quint32)sizeCharInfos; // charInfos size

    block.append(qbaAll);

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
    qint32 sizeSt;
    qint32 sizeInputRecords;

    in >> size;
    in >> sizeSt;
    in >> sizeInputRecords;

    QByteArray qbaAll;
    qbaAll.resize(size);

    int readBytes = in.readRawData(qbaAll.data(), size);
    if(readBytes == -1)
        return -1;

    qbaAll = aes.decrypt(qbaAll);
    data.inputRecords.resize(sizeInputRecords/sizeof(INPUT_RECORD));

    int pos = 0;
    ConvertorData::qbytearray_to_data(qbaAll.mid(pos, sizeSt), &data.st, sizeSt);
    pos += sizeSt;
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

    console.killAll();
    aliveState = false;
    emit closed();

    if(server && controlConnection)
    {
        qDebug() << "send signal";
        emit killServer();
    }
}
