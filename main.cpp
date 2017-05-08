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
    QObject::connect(&example, SIGNAL(closed()), &a, SIGNAL(aboutToQuit()));
    example.menu();

    return a.exec();
}

