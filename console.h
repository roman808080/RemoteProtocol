#ifndef CONSOLE_H
#define CONSOLE_H
#include <iostream>
#include <windows.h>

#include "datastruct.h"

class Console
{
public:
    Console();
    ~Console();

    int readInputFromConsole(DataIn &data);
    int writeOutputToConsole(DataOut &data);

    int writeInputToConsole(DataIn& data);
    int readOutputFromConsole(DataOut& data);

    void setName(std::wstring name);

private:
    DWORD dwProcessId;
    DWORD dwErrorId;
};

#endif // CONSOLE_H
