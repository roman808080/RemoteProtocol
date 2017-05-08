#include "console.h"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif

Console::Console()
{
    dwProcessId = 0 ;
    dwErrorId = 0;
}

Console::~Console()
{
    kill();
}

void Console::kill()
{
//    CloseDesktop(desktop);
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
    if (killed)
    {
        TerminateProcess(killed, 0);
    }
    killed = OpenProcess(PROCESS_TERMINATE, false, GetCurrentProcessId());
        if (killed)
        {
            TerminateProcess(killed, 0);
        }
}

void Console::startServer(LPWSTR desktopName)
{
    if(desktopName)
        desktop = CreateDesktopW(desktopName, 0, 0, 0, GENERIC_ALL, 0);
    FreeConsole();
    std::wstring path = L"cmd.exe";

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
    if(desktopName)
        si.lpDesktop = desktopName;

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

    setName(L"Server");
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &lastClientCSBI);
    lastClientCursorPosition = {0, 0};
}

void Console::startServer()
{
    startServer(NULL);
}

int Console::readInputFromConsole(DataIn& data)
{
    HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events = 0;
    DWORD unread = 0;

    DWORD fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
    BOOL bMode = SetConsoleMode(inputHandle, fdwMode);
    if(!bMode)
    {
        std::runtime_error("error with mode");
    }

    BOOL statusUnread = TRUE;
    statusUnread = GetNumberOfConsoleInputEvents(inputHandle, &unread);
    if(!statusUnread)
        throw std::runtime_error("GetNumberOfConsoleInputEvents failed.");

    data.inputRecords.resize(unread);
    BOOL statusRead = TRUE;
    statusRead = ReadConsoleInput(inputHandle, &data.inputRecords[0], unread, &events);
    if(!statusRead)
        throw std::runtime_error("ReadConsoleInput failed.");

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &data.st.consoleScreenBufferInfo);
    data.st.windowChanged = changedClientCSBI(data.st.consoleScreenBufferInfo);
    lastClientCSBI = data.st.consoleScreenBufferInfo;

    return 0;
}

int Console::writeOutputToConsole(DataOut& data)
{
    HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if(data.st.position.X != lastClientCursorPosition.X || data.st.position.Y != lastClientCursorPosition.Y)
    {
        BOOL setCoorsor = 0;
        setCoorsor = SetConsoleCursorPosition(hConOut, data.st.position);
        //    if(!setCoorsor) ////
        //        std::runtime_error("Set coorsor failed.");
        lastClientCursorPosition = data.st.position;
    }

//    BOOL setWindowInfo = 0;
//    setWindowInfo = SetConsoleWindowInfo(hConOut, TRUE, &data.st.srctReadRect);
//    if(!setWindowInfo) ////
//        std::runtime_error("Set windows info failed.");

    WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE),
                       &data.charInfos[0],
                       {data.st.srctReadRect.Right + 1, (data.st.srctReadRect.Bottom - data.st.srctReadRect.Top + 1)},
//                       {data.size.X, data.size.Y},
                       { 0, 0 },
                       &data.st.srctReadRect);
    return 0;
}

int Console::writeInputToConsole(DataIn& data)
{
    HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwTmp;

    BOOL bScreenSize = 0;
    bScreenSize = SetConsoleScreenBufferSize(hConOut, data.st.consoleScreenBufferInfo.dwSize);
//    if(!bScreenSize) ///
//        throw std::runtime_error("Set console screen buffer size failed.");

    dwTmp = 0;
    BOOL statusWrite = 0;
    statusWrite = WriteConsoleInput(hConIn,
                                    &data.inputRecords[0],
                                    data.inputRecords.size(),
                                    &dwTmp);
    if(!statusWrite)
        throw std::runtime_error("WriteConsoleInput failed.");

    if(data.st.windowChanged)
    {
        BOOL bWindowInfo = 0;
        bWindowInfo = SetConsoleWindowInfo(hConOut, TRUE, &data.st.consoleScreenBufferInfo.srWindow);
        if(!bWindowInfo)
            throw std::runtime_error("Set position failed.");
    }

    return 0;
}

int Console::readOutputFromConsole(DataOut& data)
{
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo = {0};
    BOOL bCsbi = 0;
    bCsbi = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);
    if(!bCsbi)
    {
        throw std::runtime_error("failed get console screen buffer info.");
    }

    data.st.srctReadRect = bufferInfo.srWindow;
    data.st.position = bufferInfo.dwCursorPosition;
    data.st.size = bufferInfo.dwSize;

    data.charInfos.resize((data.st.srctReadRect.Right + 1)*(data.st.srctReadRect.Bottom - data.st.srctReadRect.Top + 1));
    BOOL bReadConsole = 0;
    bReadConsole = ReadConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE),
                                      &data.charInfos[0],
//                                      {data.size.X, data.size.Y},
                                      {data.st.srctReadRect.Right + 1, (data.st.srctReadRect.Bottom - data.st.srctReadRect.Top + 1)},
                                      {0, 0},
                                      &bufferInfo.srWindow);
    if(!bReadConsole)
    {
         throw std::runtime_error("failed read from console.");
    }

    return 0;
}

void Console::setName(std::wstring name)
{
    SetConsoleTitle((LPCWSTR)name.c_str());
}

bool Console::changedClientCSBI(CONSOLE_SCREEN_BUFFER_INFO &csbi)
{
    return (
                csbi.srWindow.Bottom != lastClientCSBI.srWindow.Bottom ||
                csbi.srWindow.Top != lastClientCSBI.srWindow.Top ||
                csbi.srWindow.Left != lastClientCSBI.srWindow.Left ||
                csbi.srWindow.Right != lastClientCSBI.srWindow.Right
           );
}
