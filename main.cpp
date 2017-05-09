#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <QObject>

#include "example.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    char temp[10];
//    ltoa((long int)t,temp,10);
//    long long result = atol(temp);

//    qDebug() << result;
//    qDebug() << t;

    if(argc == 3)
    {
        qDebug() << argv[1] << " " << argv[2];
//        qDebug() << (atol(argv[1]) + atol(argv[2]));
        qDebug() << "-------------------------------------------------------";
    }
    Example example;
    example.menu();

    return a.exec();
}

