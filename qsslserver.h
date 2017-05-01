#ifndef QSSLSERVER_H
#define QSSLSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QScopedPointer>
#include <QSslError>

//class QSslServer: public QTcpServer
//{
//    Q_OBJECT

//public:
//    QSslServer(QObject *parent);
//    virtual ~QSslServer();
//    QSslSocket *nextPendingConnection();

//Q_SIGNALS:
//    void newEncryptedConnection();
//    void peerVerifyError(const QSslError &error);
//    void sslErrors(const QList<QSslError> &errors);

//protected:
//    virtual void incomingConnection(qintptr socketDescriptor);

//private:
//    QScopedPointer<QSslSocket> serverSocket;
//};


#include <QtNetwork/QTcpServer>
#include <QtNetwork/QSslError>
#include <QtNetwork/QSslConfiguration>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QSslServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(QSslServer)

public:
    QSslServer(QObject *parent = Q_NULLPTR);
    virtual ~QSslServer();

    void setSslConfiguration(const QSslConfiguration &sslConfiguration);
    QSslConfiguration sslConfiguration() const;

    QSslSocket *nextPendingConnection();

Q_SIGNALS:
    void sslErrors(const QList<QSslError> &errors);
    void peerVerifyError(const QSslError &error);
    void newEncryptedConnection();

protected:
    virtual void incomingConnection(qintptr socket);

private:
    QSslConfiguration m_sslConfiguration;
};

QT_END_NAMESPACE

#endif // QSSLSERVER_H
