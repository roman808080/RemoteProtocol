#include "clientconsole.h"
#include "inputconsole.h"

ClientConsole::ClientConsole()
{
    FreeConsole();

    dwProcessId = 0 ;
    dwErrorId = 0;
    path = L"cmd.exe";

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    SECURITY_ATTRIBUTES security = {
       sizeof(security), NULL, TRUE
     };

    if(CreateProcess(NULL, (LPWSTR)path.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        dwProcessId = pi.dwProcessId;
    }
    else
    {
        dwErrorId = GetLastError();
        printf("CreateProcess failed (%d).\n", dwErrorId);
        return;
    }

    Sleep(1000);

    if(!AttachConsole(pi.dwProcessId))
    {
        dwErrorId = GetLastError();
        printf( "AttachConsole failed (%d).\n", dwErrorId);
        return;
    }
}

int ClientConsole::readInputFromConsole(DataIn& data)
{
    //temporary
    std::string str = "cd ..";
//    data.inputRecords = wchars2records(str);
    INPUT_RECORD* ir = wchars2records(str);
    for(int i=0; i<str.size()+1; i++)
    {
        data.inputRecords[i] = ir[i];
    }


}

int ClientConsole::writeOutputToConsole(DataOut& data)
{
    //pass
}
