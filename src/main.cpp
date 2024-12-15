#include "memory.h"
#include "termcolor.h"
#include "utils.h"

#include <ShlObj.h>
#include <Urlmon.h>
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <shlwapi.h>
#pragma comment(lib, "urlmon.lib")

int
main(void)
{
  HANDLE                 csgoHandle{};
  std::unique_ptr<Utils> utils = std::make_unique<Utils>();
  ShellExecute(nullptr, nullptr,
               L"https://www.youtube.com/channel/UCzE68xoOC2ocLDiDqC1DPrQ",
               nullptr, nullptr, SW_SHOW); // advertisement

  SetConsoleTitleA(utils->randomString(26).c_str()); // window name

  // example ( unsafe method )
  URLDownloadToFileW(nullptr, _T(" LINK "), _T(" ENTER DLL LOCATION HERE "), 0,
                     nullptr); // function-result-should-be-used
  // end

  Sleep(1337);
  if (!std::filesystem::exists(" ENTER DLL LOCATION HERE ")) {
    // verification that dll exists
    std::cout << termcolor::white << "[" << termcolor::red << "!"
              << termcolor::white << "] Something went wrong!\n"
              << '\n';
    std::cout << termcolor::white << "[" << termcolor::yellow << ">"
              << termcolor::white << "] Press any key to exit...\n"
              << '\n';
    std::cin.get();
    exit(1);
  }

  utils->processKill(); // killing steam and game process

  WinExec(R"(C:\Program Files (x86)\Steam\steam.exe -applaunch 730)",
          0); // auto game start

  std::cout << termcolor::white << "[" << termcolor::yellow << "?"
            << termcolor::white << "] Waiting for game...\n"
            << '\n';

  while ((csgoHandle = utils->getHandle()) == INVALID_HANDLE_VALUE)
    Sleep(28500);

  constexpr char dllPath[] = " ENTER DLL LOCATION HERE ";
  void          *csgoModule = VirtualAllocEx(csgoHandle, nullptr, 0x1000,
                                             MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  WriteProcessMemory(csgoHandle, csgoModule, dllPath, sizeof(dllPath), nullptr);
  CreateRemoteThread(csgoHandle, nullptr, 0,
                     reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA),
                     csgoModule, 0,
                     nullptr); // diagnostic-cast-function-type-strictUn

  std::cout << termcolor::white << "[" << termcolor::green << "!"
            << termcolor::white << "] Injection done!\n"
            << '\n';

  Sleep(15000);
  exit(1);
}
