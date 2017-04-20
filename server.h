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
    void loop();
private:
    int write(DataOut& data);
    int read(DataIn& data);

    int writeInputToConsole(DataIn& data);
    int readOutputFromConsole(DataOut& data);

    QSharedPointer<QTcpSocket> socket;

    void run();
};

#endif // SERVER_H
