#include "qsslserver.h"

QSslServer::QSslServer(QObject *parent) :
    QTcpServer(parent)
{
}

QSslServer::~QSslServer()
{
}

void QSslServer::incomingConnection(qintptr socketDescriptor)
{
    QScopedPointer<QSslSocket> serverSocket(new QSslSocket);
    if (serverSocket->setSocketDescriptor(socketDescriptor))
    {
        addPendingConnection(serverSocket.data());

        connect(serverSocket.data(), &QSslSocket::peerVerifyError, this, &QSslServer::peerVerifyError);
        typedef void (QSslSocket::* sslErrorsSignal)(const QList<QSslError> &);
        connect(serverSocket.data(), static_cast<sslErrorsSignal>(&QSslSocket::sslErrors), this, &QSslServer::sslErrors);
        connect(serverSocket.data(), &QSslSocket::encrypted, this, &QSslServer::newEncryptedConnection);

        serverSocket->startServerEncryption();
    }
}
