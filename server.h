#ifndef SERVER_H
#define SERVER_H

#include <QSharedPointer>
#include <QTcpServer>
#include <QThread>

#include "datastruct.h"

class Server//: public QThread
{
   //Q_OBJECT
public:
    Server(QSharedPointer<QTcpSocket> socket);
    int write(DataOut& data);
    int read(DataIn& data);
    void loop();
private:
    QSharedPointer<QTcpSocket> socket;

    void run();
};

#endif // SERVER_H
