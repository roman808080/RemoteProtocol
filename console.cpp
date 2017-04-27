#include "console.h"
#include "inputconsole.h"

Console::Console()
{
    FreeConsole();

    dwProcessId = 0 ;
    dwErrorId = 0;
    std::wstring path = L"cmd.exe";
    std::wstring name = L"Hello";

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

Console::~Console()
{
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
    if (killed)
    {
        TerminateProcess(killed, 0);
    }
//        CloseHandle( pi.hProcess );
//        CloseHandle( pi.hThread );
}

int Console::readInputFromConsole(DataIn& data)
{
    //temporary
//    std::wstring str = L"chcp 65001";
//    data.inputRecords.resize(40);
//    wchars2records(str, data.inputRecords);


//    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events = 0;

//    ZeroMemory(&data, sizeof(data));

//////////////////////////////////////////////////////////////

//    while(!events)
//    {
        BOOL statusRead = TRUE;
        data.inputRecords.resize(40);
        statusRead = ReadConsoleInput(inputHandle, &data.inputRecords[0], 40, &events);

        if(!statusRead)
            throw std::runtime_error("ReadConsoleInput failed.");

        data.inputRecords.resize(events);
//    }
///////////////////////////////////////////////////////////////


 /////////////////////////////////////////////////////////////////////////////////////////
//    while (data.inputRecords[0].EventType != KEY_EVENT && data.inputRecords[0].EventType != MOUSE_EVENT)
//    {
//        Sleep(25);
//        ReadConsoleInput(inputHandle, &data.inputRecords[0], 40, &events);
//    }
    ///////////////////////////////////////////////////////////////////////////////////

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &data.consoleScreenBufferInfo);
//    data.srctWriteRect = bufferInfo.srWindow;

    return 0;
}

int Console::writeOutputToConsole(DataOut& data)
{
//    data.charInfos.resize(SIZE_CHAR_INFO_LENGTH * SIZE_CHAR_INFO_WIDTH);//?
    if(!data.charInfos.size())
    {
        throw std::runtime_error("no one char infos for writing");
    }
    HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    BOOL setCoorsor = 0;
    setCoorsor = SetConsoleCursorPosition(hConOut, data.position);
    if(!setCoorsor)
        std::runtime_error("Set coorsor failed.");

    BOOL setWindowInfo = 0;
    setWindowInfo = SetConsoleWindowInfo(hConOut, TRUE, &data.srctReadRect);
    if(!setWindowInfo)
        std::runtime_error("Set windows info failed.");

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
    HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwTmp;

    BOOL bScreenSize = 0;
    bScreenSize = SetConsoleScreenBufferSize(hConOut, data.consoleScreenBufferInfo.dwSize);
    if(!bScreenSize)
        throw std::runtime_error("Set console screen buffer size failed.");

    dwTmp = 0;
    BOOL statusWrite = 0;
    statusWrite = WriteConsoleInput(hConIn,
                                    &data.inputRecords[0],
                                    data.inputRecords.size(),
                                    &dwTmp);
    if(!statusWrite)
        throw std::runtime_error("WriteConsoleInput failed.");

//    BOOL bWindowInfo = 0;
//    bWindowInfo = SetConsoleWindowInfo(hConOut, TRUE, &data.consoleScreenBufferInfo.srWindow);
//    if(!bWindowInfo)
//        throw std::runtime_error("Set position failed.");

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

    data.srctReadRect = bufferInfo.srWindow;
    data.position = bufferInfo.dwCursorPosition;

    data.charInfos.resize(SIZE_CHAR_INFO_WIDTH * SIZE_CHAR_INFO_LENGTH);
//    ZeroMemory(&data.charInfos[0], sizeof(CHAR_INFO) * SIZE_CHAR_INFO_WIDTH * SIZE_CHAR_INFO_LENGTH);
    BOOL bReadConsole = 0;
    bReadConsole = ReadConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE),
                                      &data.charInfos[0],
                                      {SIZE_CHAR_INFO_WIDTH, SIZE_CHAR_INFO_LENGTH},
                                      {0, 0},
                                      &bufferInfo.srWindow);
    if(!bReadConsole)
    {
         throw std::runtime_error("failed read from console.");
         dwErrorId = GetLastError();
         printf("CreateProcess failed (%d).\n", dwErrorId);
         return -1;
    }

    return 0;
}

void Console::setName(std::wstring name)
{
    SetConsoleTitle((LPCWSTR)name.c_str());
}
