

#ifndef CROSS_PROCESS_HOOK_MEMORYMANAGER_H
#define CROSS_PROCESS_HOOK_MEMORYMANAGER_H

#include <optional>
#include <Windows.h>

namespace Utils ::Memory {

    /**
     * ��Ŀ�����������һ�����ݣ���СΪ 0x1000 ��
     *
     * @param ProcessHandle
     * @param Size
     * @return
     */
    std::optional<LPVOID> RemoteAllocMemory(HANDLE ProcessHandle, DWORD Size = 0x1000);

    /**
     * ��Ŀ��������ͷ� Address ��������ڴ�
     *
     * @param Address
     * @return
     */
    bool RemoteFreeMemory(HANDLE ProcessHandle, LPVOID Address);

}


#endif //CROSS_PROCESS_HOOK_MEMORYMANAGER_H
