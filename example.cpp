#include <QObject>

#include "example.h"
#include "peer.h"
#include "remoteprotocol.h"

Example::Example(){
    qDebug() << "hello hell\n";
    remoteProtocol.sayHello(QHostAddress::Broadcast, true);

    connect(&remoteProtocol, SIGNAL(peerListAdded(Peer)), this, SLOT(newUsers(Peer)), Qt::DirectConnection);
    connect(&remoteProtocol, SIGNAL(simple()), this, SLOT(newUsers()), Qt::DirectConnection);
}

Example::~Example(){

}
void Example::newUsers(Peer peer){
    qDebug() << "333\n";
}

void Example::newUsers(){
    qDebug() << "333\n";
}



