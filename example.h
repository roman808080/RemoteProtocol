#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <QObject>
#include <QTcpSocket>
#include <QSharedPointer>
#include "remoteprotocol.h"

#include "peer.h"

class Example : public QObject
{
    Q_OBJECT
public:
    Example();
    virtual ~Example();
    void menu();
    void sayHello();
    void createClient(std::__cxx11::string ip, int port);
    void createServer();

public slots:
    void newUsers(Peer peer);
    void newUsers();

    void newInConnection(QSharedPointer<QTcpSocket> socket);
    void newOutConnection(QSharedPointer<QTcpSocket> socket);
//    void receiveTextComplete(QString newMessage);

private:
//    QDataStream in;
    RemoteProtocol  remoteProtocol;
};

#endif // EXAMPLE_H
