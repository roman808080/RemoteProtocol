#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <iostream>
#include <QObject>

#include "example.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Example example;
    QObject::connect(&example, SIGNAL(closed()), &a, SLOT(quit()));
    example.menu();

    return a.exec();
}

