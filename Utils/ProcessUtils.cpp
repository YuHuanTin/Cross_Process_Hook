
#include "StringUtils.h"
#include "ProcessUtils.h"
#include "AutoMemoryManager.h"

namespace Utils {
    std::optional<DWORD> getProcessID(const std::string &ProcessName) {
        auto hSnapshot = Utils::moveHandleOwner(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
        if (!hSnapshot || hSnapshot.get() == INVALID_HANDLE_VALUE)
            return std::nullopt;

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (!Process32First(hSnapshot.get(), &pe))
            return std::nullopt;

        do {
            std::string lowerExeFile     = Utils::stringToLower(pe.szExeFile);
            std::string lowerProcessName = Utils::stringToLower(ProcessName);

            if (lowerProcessName == lowerExeFile)
                return pe.th32ProcessID;
        } while (Process32Next(hSnapshot.get(), &pe));
        return std::nullopt;
    }

    std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName) {
        auto hSnapshot = Utils::moveHandleOwner(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID));
        if (!hSnapshot || hSnapshot.get() == INVALID_HANDLE_VALUE)
            return std::nullopt;

        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);
        if (!Module32First(hSnapshot.get(), &moduleEntry))
            return std::nullopt;

        do {
            std::string lowerszModule   = Utils::stringToLower(moduleEntry.szModule);
            std::string lowerModuleName = Utils::stringToLower(DllName);

            if (lowerszModule == DllName)
                return moduleEntry;
        } while (Module32Next(hSnapshot.get(), &moduleEntry));
        return std::nullopt;
    }

}


