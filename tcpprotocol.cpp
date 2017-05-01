#include "tcpprotocol.h"

#define DEFAULT_TCP_PORT 4644

TcpProtocol::TcpProtocol()
    : mTcpServer(NULL), mCurrentSocket(NULL)
{
    setPort(DEFAULT_TCP_PORT);
}

TcpProtocol::~TcpProtocol()
{}

void TcpProtocol::runTcpServer()
{
    mTcpServer.reset(new QSslServer(this));
    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
    connect(mTcpServer.data(), SIGNAL(newEncryptedConnection()), this, SLOT(newIncomingConnection()));
    connect(mTcpServer.data(), &QSslServer::peerVerifyError, this, &TcpProtocol::peerVerifyError);
    connect(mTcpServer.data(), SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    connect(mTcpServer.data(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(sendConnectError(QAbstractSocket::SocketError)));
}

void TcpProtocol::setPort(qint16 tcp)
{
    mLocalTcpPort = tcp;
}

void TcpProtocol::connectToServer(QString ip, int port)
{
    mCurrentSocket.reset(new QSslSocket);
    connect(mCurrentSocket.data(), SIGNAL(encrypted()), this, SLOT(connected()));
//    mCurrentSocket->connectToHost(ip, port);
    mCurrentSocket->connectToHostEncrypted(ip, port);
}

void TcpProtocol::newIncomingConnection()
{
    qDebug() << "new Incoming connection";
    if (!mTcpServer->hasPendingConnections()) return;

    mCurrentSocket.reset(mTcpServer->nextPendingConnection());
    connectionHandlers.append(QSharedPointer<ConnectionHandler>(new ConnectionHandler));
    connectionHandlers.at(connectionHandlers.size() - 1)->setSocket(mCurrentSocket);
    connectionHandlers.at(connectionHandlers.size() - 1)->startServer();

    emit newInConnection(mCurrentSocket);
}

void TcpProtocol::connected()
{
    connectionHandlers.append(QSharedPointer<ConnectionHandler>(new ConnectionHandler));
    connectionHandlers.at(connectionHandlers.size() - 1)->setSocket(mCurrentSocket);

    emit newOutConnection(mCurrentSocket);
}

void TcpProtocol::sslErrors(QList<QSslError> ListError)
{
    qDebug() << "error";
    for(auto qsslerror:ListError)
    {
        qDebug() << qsslerror;
    }
}

void TcpProtocol::peerVerifyError(){
    qDebug() << "peerVerifyError";
}

void TcpProtocol::sendConnectError(QAbstractSocket::SocketError e)
{
    qDebug() << e;
}
