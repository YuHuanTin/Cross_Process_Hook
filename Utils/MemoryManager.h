

#ifndef CROSS_PROCESS_HOOK_MEMORYMANAGER_H
#define CROSS_PROCESS_HOOK_MEMORYMANAGER_H

#include <optional>
#include <Windows.h>

namespace Utils ::Memory {

    /**
     * 从目标进程中申请一块内容（最小为 0x1000 ）
     *
     * @param ProcessHandle
     * @param Size
     * @return
     */
    std::optional<LPVOID> RemoteAllocMemory(HANDLE ProcessHandle, DWORD Size = 0x1000);

    /**
     * 从目标进程中释放 Address 处申请的内存
     *
     * @param Address
     * @return
     */
    bool RemoteFreeMemory(HANDLE ProcessHandle, LPVOID Address);

}


#endif //CROSS_PROCESS_HOOK_MEMORYMANAGER_H
