

#ifndef CROSS_PROCESS_HOOK_AUTOMEMORYMANAGER_H
#define CROSS_PROCESS_HOOK_AUTOMEMORYMANAGER_H

#include <memory>
#include "Windows.h"

/**
 * 用于 unique_ptr 自动 CloseHandle OpenProcess 的句柄
 */
struct AutoDelete_GetProcessHandle {
    void operator()(HANDLE OpenProcessHandle) {
        CloseHandle(OpenProcessHandle);
    }
};

namespace Utils {
    /**
     * 获取 ProcessID 的句柄
     *
     * @param ProcessID
     * @return
     */
    std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_GetProcessHandle> getProcessHandle(DWORD ProcessID) {
        return std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_GetProcessHandle>(OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID));
    }

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
     * 获取 ProcessHandle 进程的 Address 开始的 Len 长度的数据
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
     * 获取 ProcessID 进程的 Address 开始的 Len 长度的数据
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
