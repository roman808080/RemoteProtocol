#ifndef CLIENT_H
#define CLIENT_H
#include <QSharedPointer>
#include <QTcpServer>
#include <QThread>

#include "datastruct.h"

class Client: public QThread
{
    Q_OBJECT
public:
    Client(QSharedPointer<QTcpSocket> socket);
    int write(DataIn& data);
    int read(DataOut& data);
    void loop();

private:
    QSharedPointer<QTcpSocket> socket;

    void run();
};

#endif // CLIENT_H
