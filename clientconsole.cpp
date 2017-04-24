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
//    si.wShowWindow = SW_SHOW;// show main window maybe
//    si.wShowWindow = SW_MINIMIZE;

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
    std::wstring str = L"chcp 65001";

    wchars2records(str, data.inputRecords);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events = 0;

    ZeroMemory(&data, sizeof(data));

//////////////////////////////////////////////////////////////
//    BOOL statusRead = TRUE;
//    data.inputRecords.resize(40);
//    statusRead = ReadConsoleInput(inputHandle, &data.inputRecords[0], 1, &events);

//    if(!statusRead)
//        throw std::runtime_error("ReadConsoleInput failed.");

//    data.inputRecords.resize(events);
///////////////////////////////////////////////////////////////

    do
    {
        Sleep(20);
        ReadConsoleInput(inputHandle, &data.inputRecords[0], 1, &events);

    } while (data.inputRecords[0].EventType != KEY_EVENT && data.inputRecords[0].EventType != MOUSE_EVENT);

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);
}

int ClientConsole::writeOutputToConsole(DataOut& data)
{
    data.charInfos.resize(SIZE_CHAR_INFO_LENGTH * SIZE_CHAR_INFO_WIDTH);
    WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE),
                       &data.charInfos[0],
                       { SIZE_CHAR_INFO_WIDTH, SIZE_CHAR_INFO_WIDTH },
                       { 0, 0 },
                       &data.srctReadRect);
}
