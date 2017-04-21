#ifndef CLIENTCONSOLE_H
#define CLIENTCONSOLE_H

#include <iostream>
#include <windows.h>

#include "datastruct.h"
//#include "inputconsole.h"

class ClientConsole
{
public:
    ClientConsole();
    int readInputFromConsole(DataIn &data);
    int writeOutputToConsole(DataOut &data);

private:
    DWORD dwProcessId;
    DWORD dwErrorId;
    std::wstring path;
};

#endif // CLIENTCONSOLE_H
