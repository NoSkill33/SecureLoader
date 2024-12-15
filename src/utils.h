#include <TlHelp32.h>
#include <tchar.h>

class Utils
{
public:
  DWORD       processFind(const std::wstring &processName);
  VOID        processKill();
  HANDLE      getHandle(const int perms = PROCESS_ALL_ACCESS);
  std::string randomString(const size_t length);
};
