#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdio.h>
#include <wchar.h>
#include <iostream>
#include <Windows.h>
#include <winbase.h>

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

    int compare(DataOut& data, std::vector<CHAR_INFO>& temp);
    COORD indexToCOORD(int index, int sizeLine);
    bool compareCharInfo(CHAR_INFO first,CHAR_INFO second);

private:
    DWORD dwProcessId;
    DWORD dwErrorId;
    CONSOLE_SCREEN_BUFFER_INFO lastClientCSBI;
    std::vector<CHAR_INFO> lastServerCharInfos;
};

#endif // CONSOLE_H
