#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <windows.h>
#include <TlHelp32.h>

#include <QString>
#include <vector>
#include <iostream>
namespace ProcessHandler
{
    void startProcessServer(std::vector<char> password, LPWSTR desktopName);
    void startProcessClient(std::string ip, int port, LPWSTR desktopName);

    void killAllProcessServer();
    void killAllProcessClient();
    void killSelf();

    void killAllChildren(DWORD dwProcessId);
    void killParent(DWORD dwProcessId);

    BOOL ctrlHandler(DWORD fdwCtrlType);

    std::vector<DWORD> dwProcessServerIds;
    std::vector<DWORD> dwProcessClientIds;
    HDESK desktop;


    BOOL ctrlHandler(DWORD fdwCtrlType)
    {
      switch( fdwCtrlType )
      {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
          killAllProcessClient();
          killAllProcessServer();
          return( TRUE );

        // CTRL-CLOSE: confirm that the user wants to exit.
        case CTRL_CLOSE_EVENT:
          killAllProcessServer();
          return( TRUE );

        case CTRL_BREAK_EVENT:
          killAllProcessServer();
          return TRUE;

        case CTRL_LOGOFF_EVENT:
          killAllProcessServer();
          return TRUE;

        case CTRL_SHUTDOWN_EVENT:
          killAllProcessServer();
          return TRUE;

        default:
          return TRUE;
      }
    }

    void startProcessServer(std::vector<char> password, LPWSTR desktopName)
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

    void startProcessClient(std::string ip, int port, LPWSTR desktopName)
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


    void killAllProcessClient()
    {
        for(auto client: dwProcessClientIds)
        {
            killAllChildren(client);
            killParent(client);
        }
    }

    void killAllProcessServer()
    {
        for(auto server: dwProcessServerIds)
        {
            killAllChildren(server);
            killParent(server);
        }
    }

    void killAllChildren(DWORD dwProcessId)
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

    void killParent(DWORD dwProcessId)
    {
        HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
        if (killed)
            TerminateProcess(killed, 0);
    }

    void killSelf()
    {
        killAllProcessClient();
        killAllProcessServer();

        killParent(GetCurrentProcessId());
    }
}
#endif // PROCESSHANDLER_H
