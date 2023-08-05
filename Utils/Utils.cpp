

#include "Utils.h"

namespace Utils {
    namespace AutoPtr {
        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> moveHandleOwner(HANDLE Handle) {
            return std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle>(Handle);
        }
    }

    namespace RemoteProcess {
        std::optional<DWORD> getProcessID(const std::string &ProcessName) {
            auto hSnapshot = Utils::AutoPtr::moveHandleOwner(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
            if (!hSnapshot || hSnapshot.get() == INVALID_HANDLE_VALUE)
                return std::nullopt;

            PROCESSENTRY32 pe;
            pe.dwSize = sizeof(PROCESSENTRY32);
            if (!Process32First(hSnapshot.get(), &pe))
                return std::nullopt;

            do {
                std::string lowerExeFile     = Utils::String::stringToLower(pe.szExeFile);
                std::string lowerProcessName = Utils::String::stringToLower(ProcessName);

                if (lowerProcessName == lowerExeFile)
                    return pe.th32ProcessID;
            } while (Process32Next(hSnapshot.get(), &pe));
            return std::nullopt;
        }

        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> getProcessHandle(DWORD ProcessID) {
            return Utils::AutoPtr::moveHandleOwner(OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID));
        }

        std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName) {
            auto hSnapshot = Utils::AutoPtr::moveHandleOwner(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID));
            if (!hSnapshot || hSnapshot.get() == INVALID_HANDLE_VALUE)
                return std::nullopt;

            MODULEENTRY32 moduleEntry;
            moduleEntry.dwSize = sizeof(MODULEENTRY32);
            if (!Module32First(hSnapshot.get(), &moduleEntry))
                return std::nullopt;

            do {
                std::string lowerszModule   = Utils::String::stringToLower(moduleEntry.szModule);
                std::string lowerModuleName = Utils::String::stringToLower(DllName);

                if (lowerszModule == DllName)
                    return moduleEntry;
            } while (Module32Next(hSnapshot.get(), &moduleEntry));
            return std::nullopt;
        }

        bool loadDll(HANDLE ProcessHandle, DWORD LoadlibraryAddress, const std::string &DllPath) {
            auto allocMemoryAddress = allocMemory(ProcessHandle);
            if (!allocMemoryAddress) return false;

            DWORD oldProtect = 0;
            if (!VirtualProtectEx(ProcessHandle, allocMemoryAddress.value(), 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                freeMemory(ProcessHandle, allocMemoryAddress.value());
                return false;
            }

            if (!writeMemory(ProcessHandle, allocMemoryAddress.value(), DllPath.data(), DllPath.size())) {
                freeMemory(ProcessHandle, allocMemoryAddress.value());
                return false;
            }

            HANDLE hThread = CreateRemoteThread(ProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE) LoadlibraryAddress, allocMemoryAddress.value(), 0, NULL);
            if (!hThread) {
                freeMemory(ProcessHandle, allocMemoryAddress.value());
                return false;
            }
            WaitForSingleObject(hThread, INFINITE);
            freeMemory(ProcessHandle, allocMemoryAddress.value());
            return true;
        }

        bool freeDll(HANDLE ProcessHandle, DWORD FreelibraryAddress, HMODULE DllModule) {
            HANDLE hThread = CreateRemoteThread(ProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE) FreelibraryAddress, DllModule, 0, NULL);
            if (!hThread) {
                return false;
            }
            WaitForSingleObject(hThread, INFINITE);
            return true;
        }

        std::optional<LPVOID> allocMemory(HANDLE ProcessHandle, DWORD Size) {
            LPVOID addr = VirtualAllocEx(ProcessHandle, nullptr, Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            if (!addr) return std::nullopt;
            return addr;
        }

        bool freeMemory(HANDLE ProcessHandle, LPVOID Address) {
            return VirtualFreeEx(ProcessHandle, Address, 0, MEM_RELEASE);
        }
    }

    namespace String {
        std::string stringToLower(const std::string &RawString) {
            std::string newString = RawString;
            for (auto   &one: newString) {
                if (isalpha(one) && isupper(one)) {
                    one = tolower(one);
                }
            }
            return newString;
        }
    }

}