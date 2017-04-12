#include "remoteprotocol.h"

#include <QTimer>
#include <QDataStream>

#define DEFAULT_TCP_PORT 4644


RemoteProtocol::RemoteProtocol()
    : mTcpServer(NULL), mCurrentSocket(NULL)
{
    setPort(DEFAULT_TCP_PORT);
}

RemoteProtocol::~RemoteProtocol()
{}

void RemoteProtocol::runTcpServer()
{
    mTcpServer.reset(new QTcpServer);
    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
    connect(mTcpServer.data(), SIGNAL(newConnection()), this, SLOT(newIncomingConnection()));
}

void RemoteProtocol::setPort(qint16 tcp)
{
    mLocalTcpPort = tcp;
}

void RemoteProtocol::newOutcomingConnection(QString ip, int port)
{
    mCurrentSocket.reset(new QTcpSocket);
    mCurrentSocket->connectToHost(ip, port);
    connect(mCurrentSocket.data(), SIGNAL(disconnected()), this, SLOT(closedConnectionTmp()), Qt::QueuedConnection);
    emit newOutConnection(mCurrentSocket);
}

void RemoteProtocol::newIncomingConnection()
{
    if (!mTcpServer->hasPendingConnections()) return;
    mCurrentSocket.reset(mTcpServer->nextPendingConnection());
    connect(mCurrentSocket.data(), SIGNAL(disconnected()), this, SLOT(closedConnectionTmp()), Qt::QueuedConnection);
    emit newInConnection(mCurrentSocket);
}

void RemoteProtocol::closedConnectionTmp()
{
    QTimer::singleShot(500, this, SLOT(closedConnection()));
}

void RemoteProtocol::closedConnection()
{
    if (mCurrentSocket)
       {
           mCurrentSocket->disconnect();
           mCurrentSocket->disconnectFromHost();
           mCurrentSocket->close();
           mCurrentSocket->deleteLater();
       }
}

void RemoteProtocol::sendConnectError(QAbstractSocket::SocketError e)
{
    if (mCurrentSocket)
    {
        mCurrentSocket->close();
        mCurrentSocket->deleteLater();
    }
}
