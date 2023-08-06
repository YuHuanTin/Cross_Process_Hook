

#ifndef CROSS_PROCESS_HOOK_UTILS_H
#define CROSS_PROCESS_HOOK_UTILS_H

#include <string>
#include <memory>
#include <optional>
#include <Windows.h>
#include <TlHelp32.h>

#include "MyException.h"

struct AutoDelete_CloseHandle {
    void operator()(HANDLE OpenProcessHandle) {
        CloseHandle(OpenProcessHandle);
    }
};

namespace Utils {
    namespace AutoPtr {
        template<typename ElementType>
        std::unique_ptr<ElementType[]> makeElementArray(std::size_t ElementSize) noexcept {
            return std::make_unique<ElementType[]>(ElementSize);
        }

        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> moveHandleOwner(HANDLE Handle) noexcept;
    }

    namespace RemoteProcess {
        template<typename ElementType>
        std::unique_ptr<ElementType[]> readMemory(HANDLE ProcessHandle, DWORD Address, DWORD Len) {
            auto ptr = Utils::AutoPtr::makeElementArray<ElementType>(Len);
            if (!ReadProcessMemory(ProcessHandle, (LPCVOID) Address, ptr.get(), Len, nullptr)) {
                throw MyException("ReadProcessMemory", __FUNCTION__);
            }
            return ptr;
        }

        template<typename ElementType>
        std::unique_ptr<ElementType[]> readMemory(DWORD ProcessID, DWORD Address, DWORD Len) {
            auto processHandle = getProcessHandle(ProcessID);
            return readMemory<ElementType>(processHandle.get(), Address, Len);
        }

        template<typename ElementType>
        void writeMemory(HANDLE ProcessHandle, LPVOID Address, ElementType Data[], std::size_t DataLen) {
            if (!WriteProcessMemory(ProcessHandle, Address, Data, DataLen, nullptr)) {
                throw std::runtime_error("WriteProcessMemory");
            }
        }

        template<typename ElementType>
        void writeMemory(DWORD ProcessID, LPVOID Address, ElementType Data[], std::size_t DataLen) {
            auto processHandle = getProcessHandle(ProcessID);
            return writeMemory(processHandle.get(), Address, Data, DataLen);
        }

        std::optional<DWORD> getProcessID(const std::string &ProcessName);

        std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName);

        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> getProcessHandle(DWORD ProcessID) noexcept;

        void loadDll(HANDLE ProcessHandle, DWORD LoadlibraryAddress, const std::string &DllPath);

        void freeDll(HANDLE ProcessHandle, DWORD FreelibraryAddress, HMODULE DllModule);

        LPVOID allocMemory(HANDLE ProcessHandle, DWORD Size = 0x1000);

        void freeMemory(HANDLE ProcessHandle, LPVOID Address);
    }

    namespace String {
        std::string stringToLower(const std::string &RawString) noexcept;
    }
}


#endif //CROSS_PROCESS_HOOK_UTILS_H
