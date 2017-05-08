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
    example.menu();

    return a.exec();
}

