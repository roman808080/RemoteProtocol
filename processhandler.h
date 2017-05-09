#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <windows.h>

#include <QString>
#include <vector>
#include <iostream>

class ProcessHandler
{
public:
    ProcessHandler();

    void startProcessServer(std::vector<char> password, LPWSTR desktopName);
    void startProcessClient(std::string ip, int port, LPWSTR desktopName);

private:
    DWORD dwParrentId;
    std::vector<DWORD> dwProcessIds;
    HDESK desktop;
};

#endif // PROCESSHANDLER_H
