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
    TcpProtocol server;

    if(argc == 2)
    {
        std::string passwordStr =  argv[1];
        std::vector<char> password(passwordStr.begin(), passwordStr.end());

        server.runTcpServer();
        server.setPassword(password);
    }
    else
    {
        example.menu();
    }

    return a.exec();
}

