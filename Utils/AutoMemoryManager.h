

#ifndef CROSS_PROCESS_HOOK_AUTOMEMORYMANAGER_H
#define CROSS_PROCESS_HOOK_AUTOMEMORYMANAGER_H

#include <memory>
#include <Windows.h>

/**
 * ���� unique_ptr �Զ� CloseHandle OpenProcess �ľ��
 */
struct AutoDelete_GetProcessHandle {
    void operator()(HANDLE OpenProcessHandle) {
        CloseHandle(OpenProcessHandle);
    }
};

namespace Utils {
    /**
     * ת�� HANDLE ���ͱ�������Ȩ�������Զ��رվ��
     *
     * @param Handle
     * @return
     */
    std::unique_ptr<std::remove_pointer_t<HANDLE>,AutoDelete_GetProcessHandle> moveHandleOwner(HANDLE Handle);

    /**
     * ��ȡ ProcessID �ľ��
     *
     * @param ProcessID
     * @return
     */
    std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_GetProcessHandle> getProcessHandle(DWORD ProcessID);

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
     * ��ȡ ProcessHandle ���̵� Address ��ʼ�� Len ���ȵ�����
     *
     * @tparam ElementType
     * @param ProcessHandle
     * @param Address
     * @param Len
     * @return
     */
    template<typename ElementType>
    std::unique_ptr<ElementType[]> getProcessMemoryDataByReadProcess(HANDLE ProcessHandle, DWORD Address, DWORD Len) {
        auto ptr = makeElementArray<ElementType>(Len);
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
    std::unique_ptr<ElementType[]> getProcessMemoryDataByReadProcess(DWORD ProcessID, DWORD Address, DWORD Len) {
        auto processHandle = getProcessHandle(ProcessID);
        return getProcessMemoryDataByReadProcess<ElementType>(processHandle.get(), Address, Len);
    }


}

#endif //CROSS_PROCESS_HOOK_AUTOMEMORYMANAGER_H
