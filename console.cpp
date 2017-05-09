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
    killAll();
}

void Console::killSelf()
{
//    CloseDesktop(desktop);
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
    if (killed)
        TerminateProcess(killed, 0);
    killed = OpenProcess(PROCESS_TERMINATE, false, dwParrentId);
    if (killed)
        TerminateProcess(killed, 0);
}

void Console::killAll()
{
    PROCESSENTRY32 pe;

    memset(&pe, 0, sizeof(PROCESSENTRY32));
    pe.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnap = :: CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (::Process32First(hSnap, &pe))
    {
        BOOL bContinue = TRUE;

        // kill child processes
        while (bContinue)
        {
            // only kill child processes
            if (pe.th32ParentProcessID == dwProcessId)
            {
                HANDLE hChildProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);

                if (hChildProc)
                {
                    ::TerminateProcess(hChildProc, 1);
                    ::CloseHandle(hChildProc);
                }
            }

            bContinue = ::Process32Next(hSnap, &pe);
        }

        // kill the main process
        HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
        if (killed)
            TerminateProcess(killed, 0);
    }
}

void Console::startServer(LPWSTR desktopName)
{
    dwParrentId = GetCurrentProcessId();

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

    Sleep(500);

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

    if(unread)
    {
        data.inputRecords.resize(unread);
        BOOL statusRead = TRUE;
        statusRead = ReadConsoleInput(inputHandle, &data.inputRecords[0], unread, &events);
        if(!statusRead)
            throw std::runtime_error("ReadConsoleInput failed.");
    }

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

    if(data.charInfos.size())
    {
        WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE),
                           &data.charInfos[0],
                           {data.st.srctReadRect.Right + 1, (data.st.srctReadRect.Bottom - data.st.srctReadRect.Top + 1)},
                           { 0, 0 },
                           &data.st.srctReadRect);
    }

    return 0;
}

int Console::writeInputToConsole(DataIn& data)
{
    FreeConsole();
    if(!AttachConsole(dwProcessId))
    {
        dwErrorId = GetLastError();
        printf( "AttachConsole failed (%d).\n", dwErrorId);
        return -1;
    }

    HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwTmp;

    SetConsoleTextAttribute(hConOut, data.st.consoleScreenBufferInfo.wAttributes);

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
        {
            throw std::runtime_error("Set position failed.");
        }
    }

    return 0;
}

int Console::readOutputFromConsole(DataOut& data)
{
    FreeConsole();
    if(!AttachConsole(dwProcessId))
    {
        dwErrorId = GetLastError();
        printf( "AttachConsole failed (%d).\n", dwErrorId);
        return -1;
    }

    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
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
                                      {data.st.srctReadRect.Right + 1, (data.st.srctReadRect.Bottom - data.st.srctReadRect.Top + 1)},
                                      {0, 0},
                                      &bufferInfo.srWindow);
    if(!bReadConsole)
    {
         throw std::runtime_error("failed read from console.");
    }

    if(equalCharInfos(lastServerCharInfos, data.charInfos))
        data.charInfos.resize(0);
    else
        lastServerCharInfos = data.charInfos;

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

bool Console::equalCharInfos(std::vector<CHAR_INFO> first, std::vector<CHAR_INFO> second)
{
    if(first.size() != second.size())
        return false;
    for(int i=0; i<first.size(); i++)
        if(compareCharInfo(first[i], second[i]))
            return false;
    return true;
}

bool Console::compareCharInfo(CHAR_INFO first, CHAR_INFO second)
{
    if(
           first.Attributes != second.Attributes ||
           first.Char.AsciiChar != second.Char.AsciiChar ||
           first.Char.UnicodeChar != second.Char.UnicodeChar
       )
        return false;
    else
        return true;
}
