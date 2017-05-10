#include "tcpprotocol.h"

#define DEFAULT_TCP_PORT 4644

TcpProtocol::TcpProtocol()
    : mTcpServer(NULL), mCurrentSocket(NULL)
{
    setPort(DEFAULT_TCP_PORT);
    dwParrentId = GetCurrentProcessId();
}

TcpProtocol::~TcpProtocol()
{
    mTcpServer->close();
}

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

void TcpProtocol::setPassword(std::vector<char> password)
{
    this->password = password;
}

void TcpProtocol::connectToServer(QString ip, int port)
{
    mCurrentSocket = QSharedPointer<QTcpSocket>(new QTcpSocket);
    connect(mCurrentSocket.data(), SIGNAL(connected()), this, SLOT(connected()));
    mCurrentSocket->connectToHost(ip, port);
}

void TcpProtocol::newIncomingConnection()
{
    if (!mTcpServer->hasPendingConnections()) return;

    mCurrentSocket = QSharedPointer<QTcpSocket>(mTcpServer->nextPendingConnection());

    connectionHandlers.append(QSharedPointer<ConnectionHandler>(new ConnectionHandler));
    connectionHandlers.at(connectionHandlers.size() - 1)->setSocket(mCurrentSocket);
    connectionHandlers.at(connectionHandlers.size() - 1)->setPassword(password);
    connect(connectionHandlers.at(connectionHandlers.size() - 1).data(), SIGNAL(closed()), this, SIGNAL(closed()));
    connect(connectionHandlers.at(connectionHandlers.size() - 1).data(), SIGNAL(killServer()), this, SLOT(closeServer()));
    connectionHandlers.at(connectionHandlers.size() - 1)->startServer();

    emit newInConnection(mCurrentSocket);
}

void TcpProtocol::connected()
{
    connectionHandlers.append(QSharedPointer<ConnectionHandler>(new ConnectionHandler));
    connectionHandlers.at(connectionHandlers.size() - 1)->setSocket(mCurrentSocket);
    connect(connectionHandlers.at(connectionHandlers.size() - 1).data(), SIGNAL(closed()), this, SIGNAL(closed()));

    emit newOutConnection(mCurrentSocket);
}

void TcpProtocol::closeServer()
{
    for(auto connectionHandler: connectionHandlers)
    {
        if(connectionHandler->alive())
        {
            connectionHandler->closedConnection();
        }
    }
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwParrentId);
        if (killed)
            TerminateProcess(killed, 0);
}
