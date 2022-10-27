#include <iostream>
#include <Windows.h>
#include <tchar.h>

#include <Windows.h>
#include <ShlObj.h>
#include <shlwapi.h>
#include <TlHelp32.h>
#include <filesystem>

#include <fstream>
#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <Urlmon.h>
#include "termcolor.h"
#pragma comment(lib, "urlmon.lib")
using namespace std;

DWORD proc_find(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

HANDLE get_handle(int perms = PROCESS_ALL_ACCESS) {
	DWORD pid = proc_find(L"csgo.exe");

	if (!pid)
		return INVALID_HANDLE_VALUE;

	return OpenProcess(perms, FALSE, pid);
}


bool write_memory(HANDLE hFile, LONG offset, DWORD size, LPCVOID dataBuffer)
{
	DWORD lpNumberOfBytesWritten = 0;
	DWORD retValue = 0;
	DWORD dwError = 0;

	if ((hFile != INVALID_HANDLE_VALUE) && dataBuffer)
	{
		retValue = SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
		dwError = GetLastError();

		if ((retValue == INVALID_SET_FILE_POINTER) && (dwError != NO_ERROR))
		{
			return false;
		}
		else
		{
			if (WriteFile(hFile, dataBuffer, size, &lpNumberOfBytesWritten, 0))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}

bool write_memory_new(const CHAR* file, DWORD size, LPCVOID dataBuffer)
{
	HANDLE hFile = CreateFileA(file, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		bool resultValue = write_memory(hFile, 0, size, dataBuffer);
		CloseHandle(hFile);
		return resultValue;
	}
	else
	{
		return false;
	}
}

VOID proc_kill()
{
	HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32W processEntry;
	processEntry.dwSize = sizeof(processEntry);

	if (Process32FirstW(processSnapshot, &processEntry)) {
		PCWSTR steamProcesses[] = { L"Steam.exe", L"SteamService.exe", L"steamwebhelper.exe", L"csgo.exe" };
		do {
			for (INT i = 0; i < _countof(steamProcesses); i++) {
				if (!lstrcmpiW(processEntry.szExeFile, steamProcesses[i])) {
					HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processEntry.th32ProcessID);
					if (processHandle) {
						TerminateProcess(processHandle, 0);
						CloseHandle(processHandle);
					}
				}
			}
		} while (Process32NextW(processSnapshot, &processEntry));
	}
	CloseHandle(processSnapshot);
}

std::string random_string(const size_t length)
{
	std::string r;
	static const char bet[] = { "QWERTYUIPOLKJHGFDSAZXCVBNMqwertyuioplkjhgfdsazxcvbnm1234567890!@#$%^&*(" };
	srand((unsigned)time(NULL) * 5);
	for (int i = 0; i < length; ++i)
		r += bet[rand() % (sizeof(bet) - 1)];
	return r;
}

using namespace std;
int main()
{
	// Keyauth can be added here

	HANDLE csgo_handle = get_handle();
	// Hide console window
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	SetConsoleTitleA(random_string(26).c_str());

	//URLDownloadToFile(NULL, _T("link"), _T("C:/Windows/System32/acproxyy.dll"), 0, NULL); // Can be changed to any method

	Sleep(1337);
	/*if (!filesystem::exists("C:/Windows/System32/acproxyy.dll"))
		cout << termcolor::white << "[" << termcolor::red << "!" << termcolor::white << "] Something went wrong!\n" << endl;
		cout << termcolor::white << "[" << termcolor::yellow << ">" << termcolor::white << "] Press any key to exit...\n" << endl;
		cin.get();
		exit(1);
		*/

	//proc_kill();

	//WinExec("C:\\Program FIles (x86)\\Steam\\steam.exe -applaunch 730", 0);

	cout << termcolor::white << "[" << termcolor::yellow << "?" << termcolor::white << "] Waiting for CSGO...\n" << endl;

	while ((csgo_handle = get_handle(), csgo_handle == INVALID_HANDLE_VALUE))
		Sleep(28500);

	char csgo1_mod_path[] = "C:/Windows/System32/acproxyy.dll";
	void* csgo1_module = VirtualAllocEx(csgo_handle, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(csgo_handle, csgo1_module, csgo1_mod_path, sizeof(csgo1_mod_path), nullptr);
	HANDLE csgo1_legacy_thread = CreateRemoteThread(csgo_handle, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, csgo1_module, 0, 0);

	cout << termcolor::white << "[" << termcolor::green << "!" << termcolor::white << "] Injection done!\n" << endl;

	Sleep(15000);
	std::remove("C:\\Windows\\System32\\acproxyy.dll");
	exit(1);
	return 0;
}
