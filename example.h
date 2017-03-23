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

public slots:
    void newUsers(Peer peer);
    void newUsers();

private:
    RemoteProtocol  remoteProtocol;
};

#endif // EXAMPLE_H
