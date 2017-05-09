#include "processhandler.h"

ProcessHandler::ProcessHandler()
{
    dwParrentId = GetCurrentProcessId();
}

void ProcessHandler::startProcessServer(std::vector<char> password, LPWSTR desktopName)
{
    if(desktopName)
        desktop = CreateDesktopW(desktopName, 0, 0, 0, GENERIC_ALL, 0);

    std::wstring passwordServer(password.begin(), password.end());

    std::wstring path = L"remoteprotocol.exe";
    path += L" " + passwordServer;

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
        dwProcessIds.push_back(pi.dwProcessId);
    }
    else
    {
        DWORD dwErrorId = GetLastError();
        printf("CreateProcess failed (%d).\n", dwErrorId);
        return;
    }

    Sleep(500);
}

void ProcessHandler::startProcessClient(std::string ip, int port, LPWSTR desktopName)
{
    if(desktopName)
        desktop = CreateDesktopW(desktopName, 0, 0, 0, GENERIC_ALL, 0);

    std::wstring wIp(ip.begin(), ip.end());
    wchar_t temp[10];
    _itow(port, temp, 10);


    std::wstring path = L"remoteprotocol.exe";
    path += L" " + wIp + L" ";
    path += temp;

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
        dwProcessIds.push_back(pi.dwProcessId);
    }
    else
    {
        DWORD dwErrorId = GetLastError();
        printf("CreateProcess failed (%d).\n", dwErrorId);
        return;
    }

    Sleep(500);
}
