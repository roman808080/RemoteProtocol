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

    for(auto symbol: keyVector)
    {
        std::cout << symbol;
    }
    std::cout << "\n";

    int choice;
    std::cout << "Hello in menu\n ";
    std::cout << "Input 1 for create Server\n";
    std::cout << "Input 2 for create Client\n";
    std::cout << "Input -1 for end program\n";

    std::cin >> choice;

    if(choice == 1){
        std::cout << "You're choice is server\n";
        processHandler.startProcessServer(keyVector, NULL);
    }
    else if(choice == 2)
    {
        std::string ip;
        int port;

        std::cout << "You're choice is client\n";
        ip = "127.0.0.1";
        port = 4644;
//        createClient(ip, port);
        processHandler.startProcessClient(ip, port, NULL);
    }
    else
    {
        std::cout << "Invalid comand\n";
    }

}

void Example::createClient(std::string ip, int port){
    remoteProtocol.connectToServer(QString::fromUtf8(ip.c_str()), port);
}

void Example::createServer(){
    remoteProtocol.runTcpServer();
}
