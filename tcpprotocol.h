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

    signals:
        void newOutConnection(QSharedPointer<QTcpSocket> mSock);
        void newInConnection(QSharedPointer<QTcpSocket> mSock);

    private:
        QScopedPointer<QTcpServer> mTcpServer; // Socket TCP
        QSharedPointer<QTcpSocket> mCurrentSocket; // Socket TCP current socket

        //TCP port
        qint16 mLocalTcpPort;
};

#endif // REMOTEPROTOCOL_H

