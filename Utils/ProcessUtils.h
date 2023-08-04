

#ifndef CROSS_PROCESS_HOOK_PROCESSUTILS_H
#define CROSS_PROCESS_HOOK_PROCESSUTILS_H


#include <optional>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>

namespace Utils {
    std::optional<DWORD> getProcessID(const std::string &ProcessName);

    std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName);
}

#endif //CROSS_PROCESS_HOOK_PROCESSUTILS_H
