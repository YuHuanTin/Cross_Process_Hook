

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

class AutoDelete_FreeMemory {
private:
    HANDLE m_processHandle;
    LPVOID m_allocAddr;

public:
    AutoDelete_FreeMemory(HANDLE ProcessHandle, LPVOID Address)
            : m_processHandle(ProcessHandle),
              m_allocAddr(Address) {}

    [[nodiscard]] LPVOID getAddr() const noexcept {
        return m_allocAddr;
    }

    ~AutoDelete_FreeMemory() {
        // ���ʧ�ܣ�����Ҳ�޷�����
        VirtualFreeEx(m_processHandle, m_allocAddr, 0, MEM_RELEASE);
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
         * һ����ֵ�������쳣������Ҫ�ж��Ƿ���ֵ
         * ��ȡ NumbOfType �� ElementType Ԫ��
         *
         * @tparam ElementType
         * @param ProcessHandle
         * @param Address
         * @param NumbOfType
         * @return
         */
        template<typename ElementType>
        std::unique_ptr<ElementType[]> readMemory(HANDLE ProcessHandle, LPVOID Address, std::size_t NumbOfType) {
            auto ptr = Utils::AutoPtr::makeElementArray<ElementType>(NumbOfType);
            if (!ReadProcessMemory(ProcessHandle, Address, ptr.get(), sizeof(ElementType) * NumbOfType, nullptr)) {
                throw MyException("ReadProcessMemory", GetLastError(), __FUNCTION__);
            }
            return ptr;
        }

        /**
         * һ���ɹ��������쳣
         *
         * @tparam ElementType
         * @param ProcessHandle
         * @param Address
         * @param Data
         * @param NumbOfByte
         */
        template<typename ElementType>
        void writeMemory(HANDLE ProcessHandle, LPVOID Address, ElementType Data[], std::size_t NumbOfByte) {
            if (!WriteProcessMemory(ProcessHandle, Address, Data, NumbOfByte, nullptr)) {
                throw MyException("WriteProcessMemory", GetLastError(), __FUNCTION__);
            }
        }

        std::optional<DWORD> getProcessID(const std::string &ProcessName);

        std::optional<MODULEENTRY32> getProcessModule(DWORD ProcessID, const std::string &DllName);

        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> getProcessHandle(DWORD ProcessID) noexcept;

        void loadDll(HANDLE ProcessHandle, LPVOID LoadlibraryAddr, const std::string &DllPath);

        void freeDll(HANDLE ProcessHandle, LPVOID FreelibraryAddr, HMODULE DllModule);

        /**
         * һ����ֵ�������쳣������Ҫ�ж��Ƿ���ֵ
         *
         * @param ProcessHandle
         * @param Size
         * @return
         */
        std::unique_ptr<AutoDelete_FreeMemory> allocMemory(HANDLE ProcessHandle, DWORD Size = 0x1000);
    }

    namespace String {
        std::string stringToLower(const std::string &RawString) noexcept;
    }
}


#endif //CROSS_PROCESS_HOOK_UTILS_H
