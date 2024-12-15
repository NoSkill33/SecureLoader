#include "utils.h"

DWORD
Utils::processFind(const std::wstring &processName)
{
  PROCESSENTRY32 processInfo;
  processInfo.dwSize = sizeof(processInfo);

  const HANDLE processSnapshot =
      CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  if (processSnapshot == INVALID_HANDLE_VALUE)
    return 0;

  Process32First(processSnapshot, &processInfo);
  if (processName == processInfo.szExeFile) {
    CloseHandle(processSnapshot);
    return processInfo.th32ProcessID;
  }

  while (Process32Next(processSnapshot, &processInfo)) {
    if (processName == processInfo.szExeFile) {
      CloseHandle(processSnapshot);
      return processInfo.th32ProcessID;
    }
  }

  CloseHandle(processSnapshot);
  return 0;
}

VOID
Utils::processKill()
{
  const HANDLE processSnapshot =
      CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  PROCESSENTRY32W processEntry;
  processEntry.dwSize = sizeof(processEntry);

  if (Process32FirstW(processSnapshot, &processEntry)) {
    PCWSTR processes[] = {L"Steam.exe", L"SteamService.exe",
                          L"steamwebhelper.exe", L"csgo.exe"};
    do {
      for (const auto &process : processes) {
        if (!lstrcmpiW(processEntry.szExeFile, process)) {
          if (const HANDLE processhandle = OpenProcess(
                  PROCESS_TERMINATE, FALSE, processEntry.th32ProcessID))
          {
            TerminateProcess(processhandle, 0);
            CloseHandle(processhandle);
          }
        }
      }
    } while (Process32NextW(processSnapshot, &processEntry));
  }
  CloseHandle(processSnapshot);
}

HANDLE
Utils::getHandle()
{
  const DWORD pid = this->processFind(L"csgo.exe");

  if (!pid)
    return INVALID_HANDLE_VALUE;

  return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

std::string Utils::randomString(const size_t length)
{
  string                output{};
  static constexpr char table[] = {"QWERTYUIPOLKJHGFDSAZXCVBNMqwertyuioplkjhgfdsa"
                                 "zxcvbnm1234567890!@#$%^&*("};
  srand(static_cast<unsigned>(time(nullptr)) * 5);
  for (size_t i = 0; i < length; ++i)
    r += table[rand() % (sizeof(table) - 1)]; // concurrency-mt-unsafe
  return output;
}
