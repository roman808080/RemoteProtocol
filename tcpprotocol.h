#ifndef REMOTEPROTOCOL_H
#define REMOTEPROTOCOL_H


#include <QObject>

//#include <QTcpSocket>
#include <QSslSocket>
//#include <QTcpServer>
#include <QHostInfo>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QHash>
#include <QTimer>
#include <QDataStream>

#include "peer.h"
#include "connectionhandler.h"
#include "qsslserver.h"

class TcpProtocol : public QObject
{
    Q_OBJECT

    public:
        TcpProtocol();
        virtual ~TcpProtocol();
        void runTcpServer();
        void setPort(qint16 tcp);
        void connectToServer(QString ip, int port);

    public slots:
        void newIncomingConnection();
        void connected();
        void sslErrors(QList<QSslError> ListError);
        void peerVerifyError();
        void sendConnectError(QAbstractSocket::SocketError e);

    signals:
        void newOutConnection(QSharedPointer<QSslSocket> mSock);
        void newInConnection(QSharedPointer<QSslSocket> mSock);

    private:
        QScopedPointer<QSslServer> mTcpServer; // Socket TCP
        QSharedPointer<QSslSocket> mCurrentSocket; // Socket TCP current socket
        QList<QSharedPointer<ConnectionHandler>> connectionHandlers;

        //TCP port
        qint16 mLocalTcpPort;
};

#endif // REMOTEPROTOCOL_H

