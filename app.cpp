#include <vector>
#include "app.h"
#include "randomkeygenerator.h"

#define PORT 4644

App::App()
{}

App::~App(){}

void App::menu()
{
    std::vector<char> keyVector;
    generateRandomKey(keyVector, 8);

    qDebug() << "IP this computer in network: " << publicIp();
    std::cout << "Password for connect this computer: ";
    for(auto symbol: keyVector)
    {
        std::cout << symbol;
    }
    std::cout << "\n\n";

    int choice;
    std::cout << "Input 1 for create Server\n";
    std::cout << "Input 2 for create Client\n";
    std::cout << "Input -1 for end program\n";

    while(true)
    {
        std::cout << "choice > ";
        std::cin >> choice;

        if(choice == 1){
            std::cout << "You're choice is server\n";
            processHandler.startProcessServer(keyVector, L"NEWDESKTOP");
        }
        else if(choice == 2)
        {
            std::string ip;

            std::cout << "You're choice is client\n";
            std::cout << "Input ip: ";
            std::cin >> ip;

            processHandler.startProcessClient(ip, PORT, NULL);
        }
        else
        {
            std::cout << "Invalid comand\n";
        }
    }
}


QString App::publicIp()
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
