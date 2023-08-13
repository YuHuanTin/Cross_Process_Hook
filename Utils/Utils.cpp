

#include "Utils.h"


namespace Utils {
    namespace AutoPtr {
        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> moveHandleOwner(HANDLE Handle) noexcept {
            return std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle>(Handle);
        }
    }

    namespace RemoteProcess {
        std::optional<DWORD> getProcessID(const std::string &ProcessName) {
            auto hSnapshot = Utils::AutoPtr::moveHandleOwner(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
            if (hSnapshot.get() == INVALID_HANDLE_VALUE)
                throw MyException("CreateToolhelp32Snapshot", GetLastError(), __FUNCTION__);

            PROCESSENTRY32 pe;
            pe.dwSize = sizeof(PROCESSENTRY32);
            if (!Process32First(hSnapshot.get(), &pe))
                throw MyException("Process32First", GetLastError(), __FUNCTION__);

            do {
                std::string lowerExeFile     = Utils::String::stringToLower(pe.szExeFile);
                std::string lowerProcessName = Utils::String::stringToLower(ProcessName);

                if (lowerProcessName == lowerExeFile)
                    return pe.th32ProcessID;
            } while (Process32Next(hSnapshot.get(), &pe));
            return std::nullopt;
        }

        std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName) {
            auto hSnapshot = Utils::AutoPtr::moveHandleOwner(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID));
            if (hSnapshot.get() == INVALID_HANDLE_VALUE)
                throw MyException("CreateToolhelp32Snapshot", GetLastError(), __FUNCTION__);

            MODULEENTRY32 moduleEntry;
            moduleEntry.dwSize = sizeof(MODULEENTRY32);
            if (!Module32First(hSnapshot.get(), &moduleEntry))
                throw MyException("Module32First", GetLastError(), __FUNCTION__);

            do {
                std::string lowerszModule   = Utils::String::stringToLower(moduleEntry.szModule);
                std::string lowerModuleName = Utils::String::stringToLower(DllName);

                if (lowerszModule == lowerModuleName)
                    return moduleEntry;
            } while (Module32Next(hSnapshot.get(), &moduleEntry));
            return std::nullopt;
        }

        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> getProcessHandle(DWORD ProcessID) noexcept {
            return Utils::AutoPtr::moveHandleOwner(OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID));
        }

        void loadDll(HANDLE ProcessHandle, std::size_t LoadlibraryAddr, const std::string &DllPath) {
            auto allocMemoryAddr = allocMemory(ProcessHandle);

            DWORD oldProtect = 0;
            if (!VirtualProtectEx(ProcessHandle, (LPVOID) allocMemoryAddr->getAddr(), 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                throw MyException("VirtualProtectEx", GetLastError(), __FUNCTION__);
            }

            writeMemory(ProcessHandle, allocMemoryAddr->getAddr(), DllPath.data(), DllPath.size());

            auto hThread = Utils::AutoPtr::moveHandleOwner(
                    CreateRemoteThread(ProcessHandle, nullptr, 0, (LPTHREAD_START_ROUTINE) LoadlibraryAddr, (LPVOID) allocMemoryAddr->getAddr(), 0, NULL)
            );
            if (hThread.get() == INVALID_HANDLE_VALUE) {
                throw MyException("CreateRemoteThread", GetLastError(), __FUNCTION__);
            }
            WaitForSingleObject(hThread.get(), INFINITE);
        }

        void freeDll(HANDLE ProcessHandle, std::size_t FreelibraryAddr, HMODULE DllModule) {
            auto hThread = Utils::AutoPtr::moveHandleOwner(
                    CreateRemoteThread(ProcessHandle, nullptr, 0, (LPTHREAD_START_ROUTINE) FreelibraryAddr, DllModule, 0, NULL)
            );
            if (hThread.get() == INVALID_HANDLE_VALUE)
                throw MyException("CreateRemoteThread", GetLastError(), __FUNCTION__);
            WaitForSingleObject(hThread.get(), INFINITE);
        }

        std::unique_ptr<AutoDelete_FreeMemory> allocMemory(HANDLE ProcessHandle, DWORD Size) {
            LPVOID addr = VirtualAllocEx(ProcessHandle, nullptr, Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            if (!addr)
                throw MyException("VirtualAllocEx", GetLastError(), __FUNCTION__);

            return std::make_unique<AutoDelete_FreeMemory>(ProcessHandle, (std::size_t) addr);
        }
    }

    namespace String {
        std::string stringToLower(const std::string &RawString) noexcept {
            std::string newString = RawString;
            for (auto   &one: newString) {
                if ((uint8_t) one < 0x7F && isalpha(one) && isupper(one)) {
                    one = tolower(one);
                }
            }
            return newString;
        }
    }

}