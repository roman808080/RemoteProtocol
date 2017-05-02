#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <string>
#include <iomanip>
#include <QObject>

#include <qtinyaes.h>

#include "tcpprotocol.h"
#include "peer.h"
#include "example.h"
#include "diffiehellmankeysexchanger.h"


void PrintBuffer(const std::vector<char>& buffer)
{
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        std::cout << "0x" << std::setbase(16) << std::setw(2) << std::setfill('0') << (int)(unsigned char)buffer[i] << " ";
        if ((i + 1) % 8 == 0)
        {
            std::cout << std::endl;
        }
    }
}

void PrintBuffer(const QByteArray& buffer)
{
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        std::cout << "0x" << std::setbase(16) << std::setw(2) << std::setfill('0') << (int)(unsigned char)buffer[i] << " ";
        if ((i + 1) % 8 == 0)
        {
            std::cout << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTinyAes aes;
////////////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Diffie-Hellman key exchange alogrithm example:\n";
        const size_t PModuleLength = 16;

        // Module P (The bytes representation of number 0x24357685944363427687549776543601)
        unsigned char buffer[PModuleLength] = {0x01, 0x45, 0x76, 0x87,
                                               0x99, 0x12, 0x11, 0x90,
                                               0x65, 0x65, 0x33, 0x17,
                                               0x82, 0x50, 0x71, 0x03
                                               };
//        std::vector<char> cryptoPModule((char*)buffer, (char*)buffer + PModuleLength);
//        std::vector<char> cryptoPModule((char*)buffer, (char*)buffer + PModuleLength);
        std::vector<char> cryptoPModule;
        cryptoPModule = std::vector<char>((char*)buffer, (char*)buffer + PModuleLength);
//        cryptoPModule.push_back(buffer);

        // Module G
        unsigned long cryptoGModule = 0x02;

        // Keys exchanger for Alice
        DiffieHellmanKeysExchanger<PModuleLength> aliceExchanger;
        aliceExchanger.InitDiffieHellmanKeysExchanger(cryptoPModule, cryptoGModule);

        // Keys exchanger for Bob
        DiffieHellmanKeysExchanger<PModuleLength> bobExchanger;
        bobExchanger.InitDiffieHellmanKeysExchanger(cryptoPModule, cryptoGModule);

        // Generation of exchange keys for Alice and Bob
        std::cout << "\nGeneration of exchange key for Alice...\n";
        std::vector<char> aliceExchangeKey;
        aliceExchanger.GenerateExchangeData(aliceExchangeKey);
        std::cout << "Exchange key for Alice is equal:\n";
        PrintBuffer(aliceExchangeKey);

        std::cout << "\nGeneration of exchange key for Bob...\n";
        std::vector<char> bobExchangeKey;
        bobExchanger.GenerateExchangeData(bobExchangeKey);
        std::cout << "Exchange key for Bob is equal:\n";
        PrintBuffer(bobExchangeKey);

        // Calculation if shared keys for Alice and Bob
        std::cout << "\nCalculation of shared key for Alice...\n";
        std::vector<char> aliceSharedKey;
        aliceExchanger.CompleteExchangeData(bobExchangeKey, aliceSharedKey);
        std::cout << "Shared key for Alice is equal:\n";
        PrintBuffer(aliceSharedKey);

        std::cout << "\nCalculation of shared key for Bob...\n";
        std::vector<char> bobSharedKey;
        bobExchanger.CompleteExchangeData(aliceExchangeKey, bobSharedKey);
        std::cout << "Shared key for Bob is equal:\n";
        PrintBuffer(bobSharedKey);

        if (aliceSharedKey == bobSharedKey)
        {
            std::cout << "\nOperation finished successfully: shared keys for Alice and Bob are equal.";
        }
        else
        {
            std::cout << "\nOperation failed: shared keys for Alice and Bob are different.";
        }
///////////////////////////////////////////////////////////////////////////////////////////////

    aes.setMode(QTinyAes::ECB);
    qDebug() <<  bobSharedKey.size();
    QByteArray re(&bobSharedKey[0],16);

    qDebug() << "size re " << re.size();
    PrintBuffer(re);
    aes.setKey(re);// 128 bit key -> QTinyAes::KEYSIZES must contain the size
//    aes.setIv("random_iv_128bit");//QTinyAes::BLOCKSIZE

    QByteArray cipher = aes.encrypt("very big text");
    qDebug() << aes.decrypt(cipher);



    Example example;
    example.menu();

    return a.exec();
}

