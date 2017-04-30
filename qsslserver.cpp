#include "qsslserver.h"

QSslServer::QSslServer(QObject *parent) :
    QTcpServer(parent),
    serverSocket(NULL)
{
}

QSslServer::~QSslServer()
{
}

QSslSocket *QSslServer::nextPendingConnection()
{
    return static_cast<QSslSocket *>(QTcpServer::nextPendingConnection());
}

void QSslServer::incomingConnection(qintptr socketDescriptor)
{
//    QScopedPointer<QSslSocket> serverSocket(new QSslSocket);
    serverSocket.reset(new QSslSocket);
    if (serverSocket->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << "qssl";
        addPendingConnection(serverSocket.data());

        connect(serverSocket.data(), &QSslSocket::peerVerifyError, this, &QSslServer::peerVerifyError);
        typedef void (QSslSocket::* sslErrorsSignal)(const QList<QSslError> &);
        connect(serverSocket.data(), static_cast<sslErrorsSignal>(&QSslSocket::sslErrors), this, &QSslServer::sslErrors);
        connect(serverSocket.data(), &QSslSocket::encrypted, this, &QSslServer::newEncryptedConnection);

        serverSocket->startServerEncryption();
    }
}
