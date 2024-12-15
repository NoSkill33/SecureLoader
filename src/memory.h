#include <TlHelp32.h>
#include <Windows.h>
#include <filesystem>

class Memory
{
public:
  bool write(const HANDLE handleFile, const LONG offset, const DWORD size,
             const LPCVOID dataBuffer);
  bool
  writeFinal(const CHAR *file, const DWORD size, const LPCVOID dataBuffer)
};
