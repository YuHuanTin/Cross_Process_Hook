

#ifndef CROSS_PROCESS_HOOK_UTILS_H
#define CROSS_PROCESS_HOOK_UTILS_H

#include <string>
#include <memory>
#include <optional>
#include <Windows.h>
#include <TlHelp32.h>

/**
 * ���� unique_ptr �Զ� CloseHandle OpenProcess �ľ��
 */
struct AutoDelete_CloseHandle {
    void operator()(HANDLE OpenProcessHandle) {
        CloseHandle(OpenProcessHandle);
    }
};

namespace Utils {
    namespace AutoPtr {
        /**
 * �½�һ���ڴ����ڴ��� ElementSize ��С�� ElementType ����Ԫ��
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
    * ת�� HANDLE ���ͱ�������Ȩ�������Զ��رվ��
    *
    * @param Handle
    * @return
    */
        std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_CloseHandle> moveHandleOwner(HANDLE Handle);
    }

    namespace RemoteProcess {
        /**
     * ��ȡ ProcessHandle ���̵� Address ��ʼ�� Len ���ȵ�����
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
         * ��ȡ ProcessID ���̵� Address ��ʼ�� Len ���ȵ�����
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
