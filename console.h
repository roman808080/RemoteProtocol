#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdio.h>
#include <wchar.h>
#include <iostream>
#include <Windows.h>
#include <winbase.h>
#include <QtWidgets/QMessageBox>

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
    COORD indexToCoord(int index, int sizeLine);
    int coordToIndex(int left, int top, int sizeLine);

private:
    DWORD dwProcessId;
    DWORD dwErrorId;
    CONSOLE_SCREEN_BUFFER_INFO lastClientCSBI;
    SMALL_RECT srctLastRect;
};

#endif // CONSOLE_H
