#include <vector>
#include "app.h"
#include "randomkeygenerator.h"
#include "processhandler.h"

#define PORT 4644
#define SIZE_PASSWORD 8

App::App()
{
    generateRandomKey(keyVector, SIZE_PASSWORD);

    SetConsoleCtrlHandler((PHANDLER_ROUTINE) ProcessHandler::ctrlHandler, TRUE);// set Ctrl handler
}

App::~App()
{
    ProcessHandler::killAllProcessServer();
    ProcessHandler::killSelf();
}

void App::menu()
{
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
    std::cout << "Input 3 for stop Server\n";
    std::cout << "Input 4 for refresh password\n";
    std::cout << "Input -1 for end program\n";

    while(true)
    {
        std::cout << "choice > ";
        std::cin >> choice;

        if(choice == 1){
            std::cout << "You're choice is server\n";
            startServer();
        }
        else if(choice == 2)
        {
            std::string ip;

            std::cout << "You're choice is client\n";
            std::cout << "Input ip: ";
            std::cin >> ip;

            ProcessHandler::startProcessClient(ip, PORT, NULL);
        }
        else if(choice == 3)
        {
            ProcessHandler::killAllProcessServer();
        }
        else if(choice == 4)
        {
            generateRandomKey(keyVector, SIZE_PASSWORD);
            std::string key(keyVector.begin(), keyVector.end());
            std::cout << "New password: " << key.c_str() << std::endl;
            std::cout << "For apply reboot server\n";
        }
        else if(choice == -1)
        {
            ProcessHandler::killAllProcessServer();
            ProcessHandler::killSelf();
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
        socket.disconnect();
    } else {
        qWarning()
            << "could not determine local IPv4 address:"
            << socket.errorString();
        ip = "";
    }
    return ip;
}


bool App::startServer()
{
    QTcpSocket tempSocket;
    tempSocket.connectToHost("127.0.0.1", PORT);
    if(!tempSocket.waitForConnected())
    {
        ProcessHandler::startProcessServer(keyVector, L"NEWDESKTOP");
        return true;
    }
    else
    {
        qWarning()
            << "Server is running!!!";
        tempSocket.disconnect();
        return false;
    }
}
