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
        dwProcessServerIds.push_back(pi.dwProcessId);
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
        dwProcessClientIds.push_back(pi.dwProcessId);
    }
    else
    {
        DWORD dwErrorId = GetLastError();
        printf("CreateProcess failed (%d).\n", dwErrorId);
        return;
    }

    Sleep(500);
}


void ProcessHandler::killAllProcessClient()
{
    for(auto client: dwProcessClientIds)
    {
        killAllChildren(client);
    }
}

void ProcessHandler::killAllProcessServer()
{
    for(auto server: dwProcessServerIds)
    {
        killAllChildren(server);
        killParent(server);
    }
}

void ProcessHandler::killAllChildren(DWORD dwProcessId)
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
    }
}

void ProcessHandler::killParent(DWORD dwProcessId)
{
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
    if (killed)
        TerminateProcess(killed, 0);
    killed = OpenProcess(PROCESS_TERMINATE, false, dwParrentId);
    if (killed)
        TerminateProcess(killed, 0);
}

void ProcessHandler::killSelf()
{
    killAllProcessClient();
    killAllProcessServer();

    killParent(dwParrentId);
}
