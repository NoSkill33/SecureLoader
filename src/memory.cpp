#include "memory.h"

bool
Memory::write(const HANDLE handleFile, const LONG offset, const DWORD size,
              const LPCVOID dataBuffer)
{
  DWORD bytesWritten = 0;

  if (handleFile != INVALID_HANDLE_VALUE && dataBuffer) {
    const DWORD returnValue = SetFilePointer(handleFile, offset, nullptr, FILE_BEGIN);

    if (const DWORD lastError = GetLastError();
        returnValue == INVALID_SET_FILE_POINTER && lastError != NO_ERROR)
      return false;
    if (WriteFile(handleFile, dataBuffer, size, &bytesWritten,
                  nullptr))
      return true;
    return false;
  }
  return false;
}

bool
Memory::writeFinal(const CHAR *file, const DWORD size, const LPCVOID dataBuffer)
{
  if (const HANDLE handleFile = CreateFileA(file, GENERIC_WRITE, 0, nullptr,
                                        CREATE_ALWAYS, 0, nullptr);
      handleFile != INVALID_HANDLE_VALUE)
  {
    const bool resultValue = this->write(handleFile, 0, size, dataBuffer);
    CloseHandle(handleFile);
    return resultValue;
  }
  return false;
}
