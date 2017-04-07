#include "remoteprotocol.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QNetworkInterface>
#include <QTimer>
#include <QDataStream>
#include <QDebug>


#define DEFAULT_UDP_PORT 4644
#define DEFAULT_TCP_PORT 4644


RemoteProtocol::RemoteProtocol()
    : mUdpSocket(NULL), mTcpServer(NULL), mCurrentSocket(NULL)
{
    setPorts(DEFAULT_UDP_PORT, DEFAULT_TCP_PORT);

    mIsSending = false;
    mIsReceiving = false;

//    initialize();
}

RemoteProtocol::~RemoteProtocol()
{
    if (mCurrentSocket) delete mCurrentSocket;
    if (mUdpSocket) delete mUdpSocket;
    if (mTcpServer) delete mTcpServer;
}

void RemoteProtocol::runUdpSocket()
{
    mUdpSocket = new QUdpSocket(this);
    mUdpSocket->bind(QHostAddress::Any, mLocalUdpPort);
    connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(newUdpData()));
}

void RemoteProtocol::runTcpServer()
{
    mTcpServer = new QTcpServer(this);
    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
    connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(newIncomingConnection()));
}

//void RemoteProtocol::initialize()
//{
//    mUdpSocket = new QUdpSocket(this);
//    mUdpSocket->bind(QHostAddress::Any, mLocalUdpPort);
//    connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(newUdpData()));

//    mTcpServer = new QTcpServer(this);
//    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
//    connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(newIncomingConnection()));
//}

void RemoteProtocol::setPorts(qint16 udp, qint16 tcp)
{
    mLocalUdpPort = udp;
    mLocalTcpPort = tcp;
}

void RemoteProtocol::sayHello(QHostAddress dest, qint16 port)
{
    QByteArray *packet = new QByteArray();
    if ((port == DEFAULT_UDP_PORT) && (mLocalUdpPort == DEFAULT_UDP_PORT))
    {
        if (dest == QHostAddress::Broadcast)
            packet->append(0x01);           // 0x01 -> HELLO MESSAGE (broadcast)
        else
            packet->append(0x02);           // 0x02 -> HELLO MESSAGE (unicast)
    }
    else
    {
        if (dest == QHostAddress::Broadcast)
            packet->append(0x04);           // 0x04 -> HELLO MESSAGE (broadcast) with PORT
        else
            packet->append(0x05);           // 0x05 -> HELLO MESSAGE (unicast) with PORT
        packet->append((char*)&mLocalUdpPort, sizeof(qint16));
    }
//    packet->append(getSystemSignature());

    if (dest == QHostAddress::Broadcast) {
        sendToAllBroadcast(packet, port);
        if (port != DEFAULT_UDP_PORT) sendToAllBroadcast(packet, DEFAULT_UDP_PORT);
    }
    else
        mUdpSocket->writeDatagram(packet->data(), packet->length(), dest, port);

    delete packet;
}

void RemoteProtocol::sayGoodbye()
{
    // Create packet
    QByteArray *packet = new QByteArray();
    packet->append(0x03);               // 0x03 -> GOODBYE MESSAGE
    packet->append("Bye Bye");

    // Look for all the discovered ports
    QList<qint16> ports;
    ports.append(mLocalUdpPort);
    if (mLocalUdpPort != DEFAULT_UDP_PORT) ports.append(DEFAULT_UDP_PORT);
    foreach (const Peer &p, mPeers.values())
        if (!ports.contains(p.port))
            ports.append(p.port);

    // Send broadcast message to all discovered ports
    foreach (const qint16 &port, ports)
        sendToAllBroadcast(packet, port);

    // Free memory
    delete packet;
}

void RemoteProtocol::newUdpData()
{
    while (mUdpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        const int maxLength = 65536;  // Theoretical max length in IPv4
        datagram.resize(maxLength);
        // datagram.resize(mUdpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        int size = mUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        datagram.resize(size);
        handleMessage(datagram, sender);
     }
}


void RemoteProtocol::handleMessage(QByteArray &data, QHostAddress &sender)
{
    qDebug() << sender << " " << sender.isLoopback() << " new data\n";
    char msgtype = data.at(0);

    switch(msgtype)
    {
        case 0x01:  // HELLO (broadcast)
        case 0x02:  // HELLO (unicast)
            data.remove(0, 1);
            mPeers[sender.toString()] = Peer(sender, QString::fromUtf8(data), DEFAULT_UDP_PORT);
            if (msgtype == 0x01) sayHello(sender, DEFAULT_UDP_PORT);
            emit peerListAdded(mPeers[sender.toString()]);

            break;

        case 0x03:  // GOODBYE
            emit peerListRemoved(mPeers[sender.toString()]);
            mPeers.remove(sender.toString());
            break;

        case 0x04:  // HELLO (broadcast) with PORT
        case 0x05:  // HELLO (unicast) with PORT
            data.remove(0, 1);
            qint16 port = *((qint16*) data.constData());
            data.remove(0, 2);
            mPeers[sender.toString()] = Peer(sender, QString::fromUtf8(data), port);
            if (msgtype == 0x04) sayHello(sender, port);
            emit peerListAdded(mPeers[sender.toString()]);
            break;
    }

}

void RemoteProtocol::newOutcomingConnection(QString ip, int port)
{
    if (mIsReceiving || mIsSending) return;

    mCurrentSocket = new QTcpSocket();
    mCurrentSocket->connectToHost(ip, port);

    connect(mCurrentSocket, SIGNAL(disconnected()), this, SLOT(closedConnectionTmp()), Qt::QueuedConnection);

    mIsSending = true;
}

void RemoteProtocol::newIncomingConnection()
{
//    qDebug() << "here";
    if (!mTcpServer->hasPendingConnections() || mIsReceiving || mIsSending) return;

    mCurrentSocket = mTcpServer->nextPendingConnection();

    connect(mCurrentSocket, SIGNAL(readyRead()), this, SLOT(readNewData()), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(disconnected()), this, SLOT(closedConnectionTmp()), Qt::QueuedConnection);

    emit newClientConnection();

    mIsReceiving = true;
}

void RemoteProtocol::readNewData()
{
    QDataStream in;
    in.setDevice(mCurrentSocket);
    in.setVersion(QDataStream::Qt_4_0);

    in.startTransaction();

    QString nextMessage;
    in >> nextMessage;

//    qDebug() << nextMessage <<" hereldldldld\n";

    receiveTextComplete(nextMessage);
}

void RemoteProtocol::closedConnectionTmp()
{
    QTimer::singleShot(500, this, SLOT(closedConnection()));

    mIsReceiving = false;
    mIsSending = false;
}

void RemoteProtocol::closedConnection()
{
    if (mCurrentSocket)
       {
           mCurrentSocket->disconnect();
           mCurrentSocket->disconnectFromHost();
           mCurrentSocket->close();
           mCurrentSocket->deleteLater();
           mCurrentSocket = NULL;
       }

    mIsReceiving = false;
    mIsSending = false;
}

void RemoteProtocol::sendData(QString text)
{
    if(!mCurrentSocket) return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << text;

    mCurrentSocket->write(block);

}

void RemoteProtocol::sendConnectError(QAbstractSocket::SocketError e)
{
    if (mCurrentSocket)
    {
        mCurrentSocket->close();
        mCurrentSocket->deleteLater();
        mCurrentSocket = NULL;
    }

    mIsReceiving = false;
    mIsSending = false;
}


void RemoteProtocol::sendToAllBroadcast(QByteArray *packet, qint16 port)
{
    // for all interface in this network
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    // send for all
    for (int i = 0; i < ifaces.size(); i++)
    {
        QList<QNetworkAddressEntry> addrs = ifaces[i].addressEntries();
        for (int j = 0; j < addrs.size(); j++)
            if ((addrs[j].ip().protocol() == QAbstractSocket::IPv4Protocol) && (addrs[j].broadcast().toString() != ""))
            {
                mUdpSocket->writeDatagram(packet->data(), packet->length(), addrs[j].broadcast(), port);
                mUdpSocket->flush();
            }
    }
}
