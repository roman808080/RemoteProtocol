#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <QObject>
//#include <QScopedPointer>
//#include <QSharedPointer>

#include "remoteprotocol.h"
#include "peer.h"
#include "example.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    RemoteProtocol  remoteProtocol;
//    QSharedPointer<Example> el;
//    QSharedPointer<Example> el2(el);

//    QScopedPointer<Example> el4;
//    QScopedPointer<Example> el24(el);

    Example example;
    example.menu();

//    remoteProtocol.sayHello(QHostAddress::Broadcast, true);
//    QObject::connect(&remoteProtocol, SIGNAL(peerListAdded(Peer), &example, SLOT(newUsers(Peer)));
    return a.exec();
}

