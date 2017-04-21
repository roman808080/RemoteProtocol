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
    mTcpServer.reset(new QTcpServer);
    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
    connect(mTcpServer.data(), SIGNAL(newConnection()), this, SLOT(newIncomingConnection()));
}

void TcpProtocol::setPort(qint16 tcp)
{
    mLocalTcpPort = tcp;
}

void TcpProtocol::newOutcomingConnection(QString ip, int port)
{
    mCurrentSocket.reset(new QTcpSocket);
    mCurrentSocket->connectToHost(ip, port);
    connect(mCurrentSocket.data(), SIGNAL(disconnected()), this, SLOT(closedConnectionTmp()), Qt::QueuedConnection);
    emit newOutConnection(mCurrentSocket);
}

void TcpProtocol::newIncomingConnection()
{
    if (!mTcpServer->hasPendingConnections()) return;
    mCurrentSocket.reset(mTcpServer->nextPendingConnection());
    connect(mCurrentSocket.data(), SIGNAL(disconnected()), this, SLOT(closedConnectionTmp()), Qt::QueuedConnection);
    emit newInConnection(mCurrentSocket);
}

void TcpProtocol::closedConnectionTmp()
{
    QTimer::singleShot(500, this, SLOT(closedConnection()));
}

void TcpProtocol::closedConnection()
{
    if (mCurrentSocket)
       {
           mCurrentSocket->disconnect();
           mCurrentSocket->disconnectFromHost();
           mCurrentSocket->close();
           mCurrentSocket->deleteLater();
       }
}

void TcpProtocol::sendConnectError(QAbstractSocket::SocketError e)
{
    if (mCurrentSocket)
    {
        mCurrentSocket->close();
        mCurrentSocket->deleteLater();
    }
}
