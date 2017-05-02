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
#include "connectionhandler.h"

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

    signals:
        void newOutConnection(QSharedPointer<QTcpSocket> mSock);
        void newInConnection(QSharedPointer<QTcpSocket> mSock);

    private:
        QScopedPointer<QTcpServer> mTcpServer; // Socket TCP
        QSharedPointer<QTcpSocket> mCurrentSocket; // Socket TCP current socket
        QList<QSharedPointer<ConnectionHandler>> connectionHandlers;

        //TCP port
        qint16 mLocalTcpPort;
};

#endif // REMOTEPROTOCOL_H

