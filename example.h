#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <QObject>
#include "remoteprotocol.h"

#include "peer.h"

class Example : QObject
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

    void newClientConnection();
    void receiveTextComplete(QString* newMessage);

private:
    RemoteProtocol  remoteProtocol;
};

#endif // EXAMPLE_H
