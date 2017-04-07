#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <QObject>

#include "remoteprotocol.h"
#include "peer.h"
#include "example.h"


//class Example:QObject
//{
////
//  public:
//    Example(){

//  }
//  virtual ~Example(){

//  }

//  public slots:
//    void newUsers(Peer peer){
//        qDebug() << "333\n";
//    }
//};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    RemoteProtocol  remoteProtocol;
    Example example;
    example.menu();

//    remoteProtocol.sayHello(QHostAddress::Broadcast, true);
//    QObject::connect(&remoteProtocol, SIGNAL(peerListAdded(Peer), &example, SLOT(newUsers(Peer)));
    return a.exec();
}

