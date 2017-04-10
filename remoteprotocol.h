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

#include "peer.h"

class RemoteProtocol : public QObject
{
    Q_OBJECT

    public:
        RemoteProtocol();
        virtual ~RemoteProtocol();
        void runUdpSocket();
        void runTcpServer();
        void getUsers();

        void setPorts(qint16 udp, qint16 tcp);

        void sayHello(QHostAddress dest);
        void sayHello(QHostAddress dest, qint16 port);
        void sayGoodbye();

        inline QHash<QString, Peer>& getPeers() { return mPeers; }

        void sendText(QString ipDest, qint16 port, QString text);

//        inline bool isBusy() { return mIsSending || mIsReceiving; }

    public slots:
        void newUdpData();
        void newIncomingConnection();
        void newOutcomingConnection(QString ip, int port);
//        void readNewData();
        void closedConnection();
        void closedConnectionTmp();
//        void sendData(QString text);
        void sendConnectError(QAbstractSocket::SocketError);
        void sendToAllBroadcast(QByteArray *packet, qint16 port);

    signals:
        void peerListAdded(Peer peer);
        void peerListRemoved(Peer peer);
//        void receiveTextComplete(QString newMessage);
//        void sendTextComplete(QString *text);
//        void newClientConnection(QSharedPointer<QTcpSocket> mSock);
        void newOutConnection(QSharedPointer<QTcpSocket> mSock);
        void newInConnection(QSharedPointer<QTcpSocket> mSock);

    private:
        QScopedPointer<QUdpSocket> mUdpSocket; // Socket UDP
        QScopedPointer<QTcpServer> mTcpServer; // Socket UDP
        QSharedPointer<QTcpSocket> mCurrentSocket; // Socket TCP current socket

        QHash<QString, Peer> mPeers; // List of Peers

        void handleMessage(QByteArray &data, QHostAddress &sender);


        //TCP and UDP ports
        qint16 mLocalUdpPort;
        qint16 mLocalTcpPort;

//        bool mIsSending;
//        bool mIsReceiving;

};

#endif // REMOTEPROTOCOL_H

