//#include "qsslserver.h"

//QSslServer::QSslServer(QObject *parent) :
//    QTcpServer(parent),
//    serverSocket(NULL)
//{
//}

//QSslServer::~QSslServer()
//{
//}

//QSslSocket *QSslServer::nextPendingConnection()
//{
//    return static_cast<QSslSocket *>(QTcpServer::nextPendingConnection());
//}

//void QSslServer::incomingConnection(qintptr socketDescriptor)
//{
////    QScopedPointer<QSslSocket> serverSocket(new QSslSocket);
//    serverSocket.reset(new QSslSocket);
//    if (serverSocket->setSocketDescriptor(socketDescriptor))
//    {
//        qDebug() << "qssl";
//        addPendingConnection(serverSocket.data());

//        connect(serverSocket.data(), &QSslSocket::peerVerifyError, this, &QSslServer::peerVerifyError);
//        typedef void (QSslSocket::* sslErrorsSignal)(const QList<QSslError> &);
//        connect(serverSocket.data(), static_cast<sslErrorsSignal>(&QSslSocket::sslErrors), this, &QSslServer::sslErrors);
//        connect(serverSocket.data(), &QSslSocket::encrypted, this, &QSslServer::newEncryptedConnection);

//        serverSocket->startServerEncryption();
//    }
//}


#include "qsslserver.h"

#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslCipher>

QT_BEGIN_NAMESPACE

QSslServer::QSslServer(QObject *parent) :
    QTcpServer(parent),
    m_sslConfiguration(QSslConfiguration::defaultConfiguration())
{
}

QSslServer::~QSslServer()
{
}

void QSslServer::setSslConfiguration(const QSslConfiguration &sslConfiguration)
{
    m_sslConfiguration = sslConfiguration;
}

QSslConfiguration QSslServer::sslConfiguration() const
{
    return m_sslConfiguration;
}

QSslSocket *QSslServer::nextPendingConnection()
{
    return static_cast<QSslSocket *>(QTcpServer::nextPendingConnection());
}

void QSslServer::incomingConnection(qintptr socket)
{
    QSslSocket *pSslSocket = new QSslSocket();
    pSslSocket->ignoreSslErrors();

    if (Q_LIKELY(pSslSocket)) {
        pSslSocket->setSslConfiguration(m_sslConfiguration);

        if (Q_LIKELY(pSslSocket->setSocketDescriptor(socket))) {
            connect(pSslSocket, &QSslSocket::peerVerifyError, this, &QSslServer::peerVerifyError);

            typedef void (QSslSocket::* sslErrorsSignal)(const QList<QSslError> &);
            connect(pSslSocket, static_cast<sslErrorsSignal>(&QSslSocket::sslErrors),
                    this, &QSslServer::sslErrors);
            connect(pSslSocket, &QSslSocket::encrypted, this, &QSslServer::newEncryptedConnection);
            connect(pSslSocket, &QSslSocket::encrypted, this, &QSslServer::newEncryptedConnection);
            connect(pSslSocket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
                [=](QAbstractSocket::SocketError socketError){ qDebug() << "error " << socketError; });

            addPendingConnection(pSslSocket);

            pSslSocket->startServerEncryption();
        } else {
           delete pSslSocket;
        }
    }
}

QT_END_NAMESPACE
