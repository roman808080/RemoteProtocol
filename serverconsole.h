#ifndef SERVERCONSOLE_H
#define SERVERCONSOLE_H

#include "datastruct.h"

#include <windows.h>
#include <iostream>


class ServerConsole
{
public:
    ServerConsole();
    ~ServerConsole();
    int writeInputToConsole(DataIn& data);
    int readOutputFromConsole(DataOut& data);

private:
    DWORD dwProcessId;
    DWORD dwErrorId;
    std::wstring path;
};

#endif // SERVERCONSOLE_H
