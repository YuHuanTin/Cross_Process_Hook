

#ifndef CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H
#define CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H

#include <memory>
#include <optional>
#include <Windows.h>


#include "Data_Struct.h"

class ControlBlockManager {
private:
    std::unique_ptr<ControlBlock> m_controlBlockPtr;

    DWORD  m_processID;
    HANDLE m_processHandle;

    // 保存哪些dll是因为hook所需才被加载的，析构的时候请一并释放
    std::vector<std::string> m_hookLoadDlls;
    DWORD                    m_freeLibraryAddr;

    std::optional<LPVOID> m_controlBlockRemoteAddr;
public:
    ControlBlockManager(DWORD ProcessID);

    void fillSocketArgs();

    void injectControlBlock(HANDLE ProcessHandle);

    std::optional<LPVOID> getRemoteControlBlockAddr();

    ~ControlBlockManager();
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H
