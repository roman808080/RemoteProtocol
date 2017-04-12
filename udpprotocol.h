#ifndef UDPPROTOCOL_H
#define UDPPROTOCOL_H

#include <QObject>

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostInfo>
#include <QScopedPointer>
#include <QHash>

#include "peer.h"

class UdpProtocol:QObject
{
    Q_OBJECT
public:
    UdpProtocol();
    virtual ~UdpProtocol();

    void runUdpSocket();
    void setPort(qint16 udp);

    void sayHello(QHostAddress dest);
    void sayHello(QHostAddress dest, qint16 port);
    void sayGoodbye();

    inline QHash<QString, Peer>& getPeers() { return mPeers; }

public slots:
    void newUdpData();
    void sendToAllBroadcast(QByteArray *packet, qint16 port);

signals:
    void peerListAdded(Peer peer);
    void peerListRemoved(Peer peer);

private:
    void handleMessage(QByteArray &data, QHostAddress &sender);

    QScopedPointer<QUdpSocket> mUdpSocket; // Socket UDP
    qint16 mLocalUdpPort;
    QHash<QString, Peer> mPeers; // List of Peers
};

#endif // UDPPROTOCOL_H
