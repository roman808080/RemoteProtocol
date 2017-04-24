#include "console.h"
#include "inputconsole.h"

Console::Console()
{
    FreeConsole();

    dwProcessId = 0 ;
    dwErrorId = 0;

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

    if(CreateProcess(NULL, (LPWSTR)L"cmd.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
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

Console::~Console()
{
    printf("we are here\n");
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
    if (killed)
    {
        TerminateProcess(killed, 0);
    }
    //    CloseHandle( pi.hProcess );
    //    CloseHandle( pi.hThread );
}

int Console::readInputFromConsole(DataIn& data)
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

    return 0;
}

int Console::writeOutputToConsole(DataOut& data)
{
    data.charInfos.resize(SIZE_CHAR_INFO_LENGTH * SIZE_CHAR_INFO_WIDTH);
    WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE),
                       &data.charInfos[0],
                       { SIZE_CHAR_INFO_WIDTH, SIZE_CHAR_INFO_WIDTH },
                       { 0, 0 },
                       &data.srctReadRect);

    return 0;
}

int Console::writeInputToConsole(DataIn& data)
{
    HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwTmp;

    dwTmp = 0;
    BOOL statusWrite;
    statusWrite = WriteConsoleInput(hConIn, &data.inputRecords[0], data.inputRecords.size(), &dwTmp);
    if(!statusWrite)
        throw std::runtime_error("WriteConsoleInput failed.");
//    Sleep(1000);

    return 0;
}

int Console::readOutputFromConsole(DataOut& data)
{
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo = {0};
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);

    data.srctReadRect = bufferInfo.srWindow;
    data.position = bufferInfo.dwCursorPosition;

    ZeroMemory(&data.charInfos, sizeof(CHAR_INFO) * SIZE_CHAR_INFO_WIDTH * SIZE_CHAR_INFO_LENGTH);

    if(!ReadConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE),
                           &data.charInfos[0],
                           {SIZE_CHAR_INFO_WIDTH, SIZE_CHAR_INFO_LENGTH},
                           {0, 0},
                           &bufferInfo.srWindow))
    {
         dwErrorId = GetLastError();
         printf("CreateProcess failed (%d).\n", dwErrorId);
         return -1;
    }

    return 0;
}
