#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <qtinyaes.h>

#include <exception>

#include "datastruct.h"
#include "console.h"
#include "convertordata.h"
#include "diffiehellmankeysexchanger.h"

#define MODULE_LENGTH 16

class ConnectionHandler:public QObject
{
    Q_OBJECT
public:
    ConnectionHandler();
    ConnectionHandler(QSharedPointer<QTcpSocket> socket);
    ~ConnectionHandler();

    void startServer();

    void setSocket(QSharedPointer<QTcpSocket> socket);
    void setSocketDescriptor(qintptr descriptor);
    void setPassword(std::vector<char> password);

public slots:
    void sendConnectError(QAbstractSocket::SocketError e);
    void closedConnection();

    void readyRead();

private:
    //exchange key
    int write(int code);
    int write(std::vector<char>& keyExchange, int code);
    int read(std::vector<char>& keyExchange);

    int writePassword(std::vector<char>& keyExchange, int code);
    int readPassword(std::vector<char>& keyExchange);

    // client side
    int write(DataIn& data);
    int read(DataOut& data);

    //server side
    int write(DataOut& data);
    int read(DataIn& data);

    qint32 readBufferSize;
    qint32 type;
    std::vector<char> cryptoPModule;
    unsigned long cryptoGModule;
    bool authorization;
    QTinyAes aes;
    Console console;
    QSharedPointer<QTcpSocket> socket;
    DiffieHellmanKeysExchanger<MODULE_LENGTH> exchanger;
    std::vector<char> key;
    std::vector<char> password;
};

#endif // CONNECTIONHANDLER_H
