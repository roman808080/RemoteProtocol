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
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
    if (killed)
    {
        TerminateProcess(killed, 0);
    }
}

void Console::startServer(LPWSTR desktopName)
{
    if(desktopName)
        CreateDesktopW(desktopName, 0, 0, 0, GENERIC_ALL, 0);
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
    SMALL_RECT writeArea{0, 0, data.st.size.X, data.st.size.Y};
    HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    BOOL setCoorsor = 0;
    setCoorsor = SetConsoleCursorPosition(hConOut, data.st.position);
//    if(!setCoorsor) ////
//        std::runtime_error("Set coorsor failed.");

    BOOL setWindowInfo = 0;
    setWindowInfo = SetConsoleWindowInfo(hConOut, TRUE, &data.st.srctReadRect);
//    if(!setWindowInfo) ////
//        std::runtime_error("Set windows info failed.");

    if(data.st.changedCharInfos)
    {
        WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE),
                           &data.charInfos[0],
                           data.st.writeTo,
                           data.st.writeFrom,
                           &writeArea);
    }
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
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    SMALL_RECT writeArea{0, 0, data.st.size.X, data.st.size.Y};
    BOOL bCsbi = 0;
    bCsbi = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);
    if(!bCsbi)
    {
        throw std::runtime_error("failed get console screen buffer info.");
    }

    data.st.srctReadRect = bufferInfo.srWindow;
    data.st.position = bufferInfo.dwCursorPosition;
    data.st.size = bufferInfo.dwSize;

    if(lastServerCharInfos.size() == 0)
    {
        lastServerCharInfos.resize((data.st.size.X - 1) * (data.st.size.Y - 1));
        BOOL bReadConsole = 0;

        bReadConsole = ReadConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE),
                                          &lastServerCharInfos[0],
                                          {data.st.size.X - 1, data.st.size.Y - 1},
                                          {0, 0},
                                          &writeArea);
        if(!bReadConsole)
        {
             dwErrorId = GetLastError();
             throw std::runtime_error("failed read from console.");
        }

        std::vector<CHAR_INFO>::const_iterator first = lastServerCharInfos.begin();
        std::vector<CHAR_INFO>::const_iterator last = lastServerCharInfos.begin()+(data.st.srctReadRect.Right+1)*(data.st.srctReadRect.Bottom+1);
        data.charInfos = std::vector<CHAR_INFO>(first, last);

        data.st.writeFrom = {0, 0};
        data.st.writeTo = {data.st.size.X - 1, data.st.size.Y - 1};

        return 0;
    }

    std::vector<CHAR_INFO> tempServerCharInfos;
    tempServerCharInfos.resize((data.st.size.X - 1) * (data.st.size.Y - 1));
    BOOL bReadConsole = 0;

    bReadConsole = ReadConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE),
                                      &tempServerCharInfos[0],
                                      {data.st.size.X - 1, data.st.size.Y - 1},
                                      {0, 0},
                                      &writeArea);
    if(!bReadConsole)
    {
         throw std::runtime_error("failed read from console.");
    }
    compare(data, tempServerCharInfos);

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

int Console::compare(DataOut &data, std::vector<CHAR_INFO> &temp)
{
   int copyFrom = 0;
   int copyTo = 0;

   if(lastServerCharInfos.size() < temp.size())
   {
       copyTo = temp.size();
   }
   else
   {
       for(int i=lastServerCharInfos.size()-1; i >= 0; i--)
       {
           if(compareCharInfo(temp[i], lastServerCharInfos[i]))
           {
               copyTo = i + 1;
               break;
           }
       }
   }

   if(copyTo)
   {
       for(int i=0; i<lastServerCharInfos.size() && i<copyTo; i++)
       {
           if(compareCharInfo(temp[i], lastServerCharInfos[i]))
           {
               copyFrom = i;
               break;
           }
       }
   }
   else
   {
       data.st.writeFrom = {-1, -1};
       data.st.writeTo = {-1, -1};
       data.st.changedCharInfos = false;

       return 0;
   }

   std::vector<CHAR_INFO>::const_iterator first = temp.begin() + copyFrom;
   std::vector<CHAR_INFO>::const_iterator last = temp.begin() + copyTo;
   data.charInfos = std::vector<CHAR_INFO>(first, last);

   data.st.writeFrom = indexToCOORD(copyFrom, data.st.size.X);
   data.st.writeTo = indexToCOORD(copyTo, data.st.size.X);
   data.st.changedCharInfos = true;

   return 0;
}

COORD Console::indexToCOORD(int index, int sizeLine)
{
    if(!index)
        return {0, 0};
    return {index % sizeLine - 1,index / sizeLine};
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
