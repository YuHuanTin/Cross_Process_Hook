

#ifndef CROSS_PROCESS_HOOK_UTILS_H
#define CROSS_PROCESS_HOOK_UTILS_H

#include <string>
#include <memory>
#include <optional>
#include <Windows.h>
#include <TlHelp32.h>

/**
 * 用于 unique_ptr 自动 CloseHandle OpenProcess 的句柄
 */
struct AutoDelete_CloseHandle {
    void operator()(HANDLE OpenProcessHandle) {
        CloseHandle(OpenProcessHandle);
    }
};

namespace Utils {
    namespace AutoPtr {
        /**
 * 新建一块内存用于储存 ElementSize 大小的 ElementType 类型元素
 *
 * @tparam ElementType
 * @param ElementSize
 * @return
 */
        template<typename ElementType>
        std::unique_ptr<ElementType[]> makeElementArray(std::size_t ElementSize) {
            return std::make_unique<ElementType[]>(ElementSize);
        }

        /**
    * 转移 HANDLE 类型变量所有权，用于自动关闭句柄
    *
    * @param Handle
    * @return
    */
        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> moveHandleOwner(HANDLE Handle);
    }

    namespace RemoteProcess {
        /**
     * 获取 ProcessHandle 进程的 Address 开始的 Len 长度的数据
     *
     * @tparam ElementType
     * @param ProcessHandle
     * @param Address
     * @param Len
     * @return
     */
        template<typename ElementType>
        std::unique_ptr<ElementType[]> readMemory(HANDLE ProcessHandle, DWORD Address, DWORD Len) {
            auto ptr = Utils::AutoPtr::makeElementArray<ElementType>(Len);
            if (!ReadProcessMemory(ProcessHandle, (LPCVOID) Address, ptr.get(), Len, nullptr)) {
                return {};
            }
            return ptr;
        }

        /**
         * 获取 ProcessID 进程的 Address 开始的 Len 长度的数据
         *
         * @tparam ElementType
         * @param ProcessID
         * @param Address
         * @param Len
         * @return
         */
        template<typename ElementType>
        std::unique_ptr<ElementType[]> readMemory(DWORD ProcessID, DWORD Address, DWORD Len) {
            auto processHandle = getProcessHandle(ProcessID);
            return readMemory<ElementType>(processHandle.get(), Address, Len);
        }

        template<typename ElementType>
        bool writeMemory(HANDLE ProcessHandle, LPVOID Address, ElementType Data[], std::size_t DataLen) {
            if (!WriteProcessMemory(ProcessHandle, Address, Data, DataLen, nullptr)) {
                return false;
            }
            return true;
        }

        template<typename ElementType>
        bool writeMemory(DWORD ProcessID, LPVOID Address, ElementType Data[], std::size_t DataLen) {
            auto processHandle = getProcessHandle(ProcessID);
            return writeMemory(processHandle.get(), Address, Data, DataLen);
        }

        std::optional<DWORD> getProcessID(const std::string &ProcessName);

        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> getProcessHandle(DWORD ProcessID);

        std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName);

        bool loadDll(HANDLE ProcessHandle, DWORD LoadlibraryAddress, const std::string &DllPath);

        bool freeDll(HANDLE ProcessHandle, DWORD FreelibraryAddress, HMODULE DllModule);

        std::optional<LPVOID> allocMemory(HANDLE ProcessHandle, DWORD Size = 0x1000);

        bool freeMemory(HANDLE ProcessHandle, LPVOID Address);
    }

    namespace String {
        std::string stringToLower(const std::string &RawString);
    }
}


#endif //CROSS_PROCESS_HOOK_UTILS_H
