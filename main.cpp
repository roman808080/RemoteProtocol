#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <QObject>

#include <qtinyaes.h>

#include "tcpprotocol.h"
#include "peer.h"
#include "example.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTinyAes aes;

    aes.setMode(QTinyAes::CBC);
    aes.setKey("randomkey_128bit");// 128 bit key -> QTinyAes::KEYSIZES must contain the size
    aes.setIv("random_iv_128bit");//QTinyAes::BLOCKSIZE

    Example example;
    example.menu();

    return a.exec();
}

