#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <windows.h>
#include <TlHelp32.h>

#include <QString>
#include <QDebug>
#include <vector>
#include <iostream>

class ProcessHandler
{
public:
    ProcessHandler();

    void startProcessServer(std::vector<char> password, LPWSTR desktopName);
    void startProcessClient(std::string ip, int port, LPWSTR desktopName);

    void killAllProcessServer();
    void killAllProcessClient();
    void killSelf();

    void killAllChildren(DWORD dwProcessId);
    void killParent(DWORD dwProcessId);

private:
    DWORD dwParrentId;
    std::vector<DWORD> dwProcessServerIds;
    std::vector<DWORD> dwProcessClientIds;
    HDESK desktop;
};

#endif // PROCESSHANDLER_H
