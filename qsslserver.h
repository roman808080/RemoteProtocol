#ifndef QSSLSERVER_H
#define QSSLSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QScopedPointer>
#include <QSslError>

class QSslServer: public QTcpServer
{
    Q_OBJECT

public:
    QSslServer(QObject *parent);
    virtual ~QSslServer();

signals:
    void encrypted();
    void peerVerifyError(const QSslError &error);
    void sslErrors(const QList<QSslError> &errors);

protected:
    virtual void incomingConnection(qintptr socketDescriptor);
};

#endif // QSSLSERVER_H
