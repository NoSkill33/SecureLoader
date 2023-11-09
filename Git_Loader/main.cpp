#include <filesystem>
#include <fstream>
#include <iostream>
#include <ShlObj.h>
#include <shlwapi.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Urlmon.h>
#include <Windows.h>

#include "termcolor.h"
#pragma comment(lib, "urlmon.lib")
using namespace std;

DWORD procFind(const std::wstring& processName)
{
    PROCESSENTRY32 process_info;
    process_info.dwSize = sizeof(process_info);

    const HANDLE processes_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processes_snapshot == INVALID_HANDLE_VALUE) return 0;

    Process32First(processes_snapshot, &process_info);
    if (processName == process_info.szExeFile)
    {
        CloseHandle(processes_snapshot);
        return process_info.th32ProcessID;
    }

    while (Process32Next(processes_snapshot, &process_info))
    {
        if (processName == process_info.szExeFile)
        {
            CloseHandle(processes_snapshot);
            return process_info.th32ProcessID;
        }
    }

    CloseHandle(processes_snapshot);
    return 0;
}

HANDLE getHandle(const int perms = PROCESS_ALL_ACCESS)
{
    const DWORD pid = procFind(L"csgo.exe");

    if (!pid) return INVALID_HANDLE_VALUE;

    return OpenProcess(perms, FALSE, pid);
}

bool writeMemory(const HANDLE hFile, const LONG offset, const DWORD size, const LPCVOID dataBuffer)
{
    DWORD lp_number_of_bytes_written = 0;

    if (hFile != INVALID_HANDLE_VALUE && dataBuffer)
    {
        const DWORD ret_value = SetFilePointer(hFile, offset, nullptr, FILE_BEGIN);

        if (const DWORD dw_error = GetLastError(); ret_value == INVALID_SET_FILE_POINTER && dw_error != NO_ERROR) return false;
        if (WriteFile(hFile, dataBuffer, size, &lp_number_of_bytes_written, nullptr)) return true;
        return false;
    }
    return false;
}

bool writeMemoryNew(const CHAR* file, const DWORD size, const LPCVOID dataBuffer)
{
    if (const HANDLE h_file = CreateFileA(file, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr); h_file != INVALID_HANDLE_VALUE)
    {
        const bool result_value = writeMemory(h_file, 0, size, dataBuffer);
        CloseHandle(h_file);
        return result_value;
    }
    return false;
}

VOID procKill()
{
    const HANDLE process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32W process_entry;
    process_entry.dwSize = sizeof(process_entry);

    if (Process32FirstW(process_snapshot, &process_entry))
    {
        PCWSTR processes[] = {L"Steam.exe", L"SteamService.exe", L"steamwebhelper.exe", L"csgo.exe"};
        do
        {
            for (const auto& process : processes)
            {
                if (!lstrcmpiW(process_entry.szExeFile, process))
                {
                    if (const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process_entry.th32ProcessID))
                    {
                        TerminateProcess(process_handle, 0);
                        CloseHandle(process_handle);
                    }
                }
            }
        }
        while (Process32NextW(process_snapshot, &process_entry));
    }
    CloseHandle(process_snapshot);
}

string randomString(const size_t length)
{
    string r;
    static constexpr char BET[] = {"QWERTYUIPOLKJHGFDSAZXCVBNMqwertyuioplkjhgfdsazxcvbnm1234567890!@#$%^&*("};
    srand(static_cast<unsigned>(time(nullptr)) * 5);
    for (size_t i = 0; i < length; ++i) r += BET[rand() % (sizeof(BET) - 1)]; // concurrency-mt-unsafe
    return r;
}

int main()
{
    HANDLE csgo_handle;
    ShellExecute(nullptr, nullptr, L"https://www.youtube.com/channel/UCzE68xoOC2ocLDiDqC1DPrQ", nullptr, nullptr, SW_SHOW); // advertisement

    SetConsoleTitleA(randomString(26).c_str()); // window name

    // example ( unsafe method )
    URLDownloadToFileW(nullptr, _T(" LINK "), _T(" ENTER DLL LOCATION HERE "), 0, nullptr); // function-result-should-be-used
    // end

    Sleep(1337);
    if (!filesystem::exists(" ENTER DLL LOCATION HERE "))
    {
        // verification that dll exists
        cout << termcolor::white << "[" << termcolor::red << "!" << termcolor::white << "] Something went wrong!\n" << endl;
        cout << termcolor::white << "[" << termcolor::yellow << ">" << termcolor::white << "] Press any key to exit...\n" << endl;
        cin.get();
        exit(1); // concurrency-mt-unsafe
    }
    
    procKill(); // killing steam and game process

    WinExec(R"(C:\Program Files (x86)\Steam\steam.exe -applaunch 730)", 0); // auto game start

    cout << termcolor::white << "[" << termcolor::yellow << "?" << termcolor::white << "] Waiting for game...\n" << endl;

    while ((csgo_handle = getHandle()) == INVALID_HANDLE_VALUE) Sleep(28500);

    constexpr char csgo1_mod_path[] = " ENTER DLL LOCATION HERE ";
    void* csgo1_module = VirtualAllocEx(csgo_handle, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(csgo_handle, csgo1_module, csgo1_mod_path, sizeof(csgo1_mod_path), nullptr);
    CreateRemoteThread(csgo_handle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), csgo1_module, 0, nullptr); // diagnostic-cast-function-type-strictUn

    cout << termcolor::white << "[" << termcolor::green << "!" << termcolor::white << "] Injection done!\n" << endl;

    Sleep(15000);
    exit(1); // concurrency-mt-unsafe
}
