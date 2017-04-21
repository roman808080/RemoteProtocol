#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QSharedPointer>
#include <QDataStream>
#include <QDebug>

#include <iostream>

#include "convertordata.h"
#include "datastruct.h"
#include "clientconsole.h"

class Client//: public QThread
{
    //Q_OBJECT
public:
    Client(QSharedPointer<QTcpSocket> socket);
    void loop();

private:
    int write(DataIn& data);
    int read(DataOut& data);

//    int readInputFromConsole(DataIn &data);
//    int writeOutputToConsole(DataOut &data);

    QSharedPointer<QTcpSocket> socket;

    void run();
};

#endif // CLIENT_H
