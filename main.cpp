#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <QObject>

#include "app.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    App app;
    QScopedPointer<ConnectionHandler> connectionHandler;
    TcpProtocol tcpProtocol;

    if(argc == 2) // start server
    {
        std::string passwordStr =  argv[1];
        std::vector<char> password(passwordStr.begin(), passwordStr.end());

        tcpProtocol.runTcpServer();
        tcpProtocol.setPassword(password);
    }
    else if(argc == 3) // start client
    {
        std::string ip =  argv[1];
        int port = atoi(argv[2]);

        tcpProtocol.connectToServer(QString::fromUtf8(ip.c_str()), port);
    }
    else // start menu
    {
        app.menu();
    }

    return a.exec();
}

