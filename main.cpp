#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <QObject>

#include "example.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Example example;
    QScopedPointer<ConnectionHandler> connectionHandler;
    TcpProtocol tcpProtocol;

    if(argc == 2)
    {
        std::string passwordStr =  argv[1];
        std::vector<char> password(passwordStr.begin(), passwordStr.end());

        tcpProtocol.runTcpServer();
        tcpProtocol.setPassword(password);
    }
    else if(argc == 3)
    {
        std::string ip =  argv[1];
        int port = atoi(argv[2]);

        tcpProtocol.connectToServer(QString::fromUtf8(ip.c_str()), port);
    }
    else
    {
        example.menu();
    }

    return a.exec();
}

