#include "serverconsole.h"

ServerConsole::ServerConsole()
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

ServerConsole::~ServerConsole()
{
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
    if (killed)
    {
        TerminateProcess(killed, 0);
    }
    //    CloseHandle( pi.hProcess );
    //    CloseHandle( pi.hThread );
}

int ServerConsole::writeInputToConsole(DataIn& data)
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

int ServerConsole::readOutputFromConsole(DataOut& data)
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

}
