

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

class AutoDelete_AllocMemory {
private:
    HANDLE      m_processHandle;
    std::size_t m_allocAddr;

public:
    AutoDelete_AllocMemory(HANDLE ProcessHandle, std::size_t Address)
            : m_processHandle(ProcessHandle),
              m_allocAddr(Address) {}

    [[nodiscard]] std::size_t getAddr() const noexcept {
        return m_allocAddr;
    }

    AutoDelete_AllocMemory() {
        // 如果失败，我们也无法处理
        VirtualFreeEx(m_processHandle, (LPVOID) m_allocAddr, 0, MEM_RELEASE);
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
        /**
         * 一定有值，否则异常，不需要判断是否有值
         * 读取 NumbOfType 个 ElementType 元素
         *
         * @tparam ElementType
         * @param ProcessHandle
         * @param Addr
         * @param NumbOfType
         * @return
         */
        template<typename ElementType>
        std::unique_ptr<ElementType[]> readMemory(HANDLE ProcessHandle, std::size_t Addr, std::size_t NumbOfType) {
            auto ptr = Utils::AutoPtr::makeElementArray<ElementType>(NumbOfType);
            if (!ReadProcessMemory(ProcessHandle,(LPCVOID) Addr, ptr.get(), sizeof(ElementType) * NumbOfType, nullptr)) {
                throw MyException("ReadProcessMemory", GetLastError(), __FUNCTION__);
            }
            return ptr;
        }

        /**
         * 一定成功，否则异常
         *
         * @tparam ElementType
         * @param ProcessHandle
         * @param Addr
         * @param Data
         * @param NumbOfByte
         */
        template<typename ElementType>
        void writeMemory(HANDLE ProcessHandle, std::size_t Addr, ElementType Data[], std::size_t NumbOfByte) {
            if (!WriteProcessMemory(ProcessHandle, (LPVOID) Addr, Data, NumbOfByte, nullptr)) {
                throw MyException("WriteProcessMemory", GetLastError(), __FUNCTION__);
            }
        }

        std::optional<DWORD> getProcessID(const std::string &ProcessName);

        std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName);

        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> getProcessHandle(DWORD ProcessID) noexcept;

        void loadDll(HANDLE ProcessHandle, std::size_t LoadlibraryAddr, const std::string &DllPath);

        void freeDll(HANDLE ProcessHandle, std::size_t FreelibraryAddr, HMODULE DllModule);

        /**
         * 一定有值，否则异常，不需要判断是否有值
         *
         * @param ProcessHandle
         * @param Size
         * @return
         */
        std::unique_ptr<AutoDelete_AllocMemory> allocMemory(HANDLE ProcessHandle, DWORD Size = 0x1000);
    }

    namespace String {
        std::string stringToLower(const std::string &RawString) noexcept;
    }
}


#endif //CROSS_PROCESS_HOOK_UTILS_H
