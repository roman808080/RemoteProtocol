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

//    char temp[10];
//    ltoa((long int)t,temp,10);
//    long long result = atol(temp);

//    qDebug() << result;
//    qDebug() << t;

    if(argc == 3)
    {
        std::string passwordStr =  argv[1];
        std::vector<char> password(passwordStr.begin(), passwordStr.end());

        qintptr descriptor = atol(argv[2]);

        connectionHandler.reset(new ConnectionHandler);
        connectionHandler->setSocketDescriptor(descriptor);
        connectionHandler->setPassword(password);
        connectionHandler->startServer();
    }
    else
    {
        example.menu();
    }

    return a.exec();
}

