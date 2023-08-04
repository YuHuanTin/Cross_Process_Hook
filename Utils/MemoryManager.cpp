

#include "MemoryManager.h"


namespace Utils ::Memory {

    std::optional<LPVOID> RemoteAllocMemory(HANDLE ProcessHandle, DWORD Size) {
        LPVOID addr = VirtualAllocEx(ProcessHandle, nullptr, Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!addr) return std::nullopt;
        return addr;
    }

    bool RemoteFreeMemory(HANDLE ProcessHandle, LPVOID Address) {
        return VirtualFreeEx(ProcessHandle, Address, 0, MEM_RELEASE);
    }
}