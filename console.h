#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdio.h>
#include <wchar.h>
#include <iostream>
#include <Windows.h>
#include <winbase.h>
#include <TlHelp32.h>

#include "datastruct.h"

class Console
{
public:
    Console();
    ~Console();
    void startServer(LPWSTR desktopName);
    void startServer();

    int readInputFromConsole(DataIn &data);
    int writeOutputToConsole(DataOut &data);

    int writeInputToConsole(DataIn& data);
    int readOutputFromConsole(DataOut& data);

    void setName(std::wstring name);
    bool changedClientCSBI(CONSOLE_SCREEN_BUFFER_INFO &csbi);
    bool equalCharInfos(std::vector<CHAR_INFO> first, std::vector<CHAR_INFO> second);
    bool compareCharInfo(CHAR_INFO first, CHAR_INFO second);

    void kill();
    void killAll();

private:
    DWORD dwParrentId;
    DWORD dwProcessId;
    DWORD dwErrorId;
    CONSOLE_SCREEN_BUFFER_INFO lastClientCSBI;
    COORD lastClientCursorPosition;
    std::vector<CHAR_INFO> lastServerCharInfos;
    HDESK desktop;
};

#endif // CONSOLE_H
