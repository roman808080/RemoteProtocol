#include "udpprotocol.h"

#include <QNetworkInterface>
#include <QDataStream>

#define DEFAULT_UDP_PORT 4644

UdpProtocol::UdpProtocol()
: mUdpSocket(NULL)
{
    setPort(DEFAULT_UDP_PORT);
}

UdpProtocol::~UdpProtocol()
{}

void UdpProtocol::runUdpSocket()
{
    mUdpSocket.reset(new QUdpSocket);
    mUdpSocket->bind(QHostAddress::Any, mLocalUdpPort);
    connect(mUdpSocket.data(), SIGNAL(readyRead()), this, SLOT(newUdpData()));
}


void UdpProtocol::setPort(qint16 udp)
{
    mLocalUdpPort = udp;
}

void UdpProtocol::sayHello(QHostAddress dest, qint16 port)
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

void UdpProtocol::sayGoodbye()
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

void UdpProtocol::newUdpData()
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


void UdpProtocol::handleMessage(QByteArray &data, QHostAddress &sender)
{
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

void UdpProtocol::sendToAllBroadcast(QByteArray *packet, qint16 port)
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
