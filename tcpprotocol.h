#ifndef REMOTEPROTOCOL_H
#define REMOTEPROTOCOL_H


#include <QObject>

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostInfo>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QHash>
#include <QTimer>
#include <QDataStream>

#include "peer.h"

class TcpProtocol : public QObject
{
    Q_OBJECT

    public:
        TcpProtocol();
        virtual ~TcpProtocol();
        void runTcpServer();
        void setPort(qint16 tcp);
        void sendText(QString ipDest, qint16 port, QString text);

    public slots:
        void newIncomingConnection();
        void newOutcomingConnection(QString ip, int port);
        void closedConnection();
        void closedConnectionTmp();
        void sendConnectError(QAbstractSocket::SocketError);

    signals:
        void newOutConnection(QSharedPointer<QTcpSocket> mSock);
        void newInConnection(QSharedPointer<QTcpSocket> mSock);

    private:
        QScopedPointer<QTcpServer> mTcpServer; // Socket UDP
        QSharedPointer<QTcpSocket> mCurrentSocket; // Socket TCP current socket

        //TCP port
        qint16 mLocalTcpPort;
};

#endif // REMOTEPROTOCOL_H

