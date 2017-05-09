#include <vector>
#include "example.h"
#include "randomkeygenerator.h"

Example::Example()
{}

Example::~Example(){}

void Example::menu()
{
    std::vector<char> keyVector;
    generateRandomKey(keyVector, 8);

    qDebug() << publicIp();
    for(auto symbol: keyVector)
    {
        std::cout << symbol;
    }
    std::cout << "\n";
    while(true)
    {
        int choice;
        std::cout << "Hello in menu\n ";
        std::cout << "Input 1 for create Server\n";
        std::cout << "Input 2 for create Client\n";
        std::cout << "Input -1 for end program\n";

        std::cin >> choice;

        if(choice == 1){
            std::cout << "You're choice is server\n";
            processHandler.startProcessServer(keyVector, L"NEWDESKTOP");
        }
        else if(choice == 2)
        {
            std::string ip;
            int port;

            std::cout << "You're choice is client\n";
            std::cout << "Input ip\n";
            std::cin >> ip;
//            ip = "127.0.0.1";
            port = 4644;
            processHandler.startProcessClient(ip, port, NULL);
        }
        else
        {
            std::cout << "Invalid comand\n";
        }
    }
}


QString Example::publicIp()
{
    QTcpSocket socket;
    QString ip;
    socket.connectToHost("8.8.8.8", 53); // google DNS, or something else reliable
    if (socket.waitForConnected()) {
        ip = socket.localAddress().toString();
    } else {
        qWarning()
            << "could not determine local IPv4 address:"
            << socket.errorString();
        ip = "";
    }
    return ip;
}
