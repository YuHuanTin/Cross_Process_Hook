

#ifndef CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H
#define CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H

#include <memory>
#include <optional>
#include <Windows.h>

#include "Data_Struct.h"
#include "../DllFuncRVAReader/ReadFromFile.h"

class ControlBlockManager {
private:
    // 控制块的指针
    std::unique_ptr<ControlBlock>  m_controlBlockPtr;
    // rva reader 指针
    std::unique_ptr<RVAReaderBase> m_rvaReader;

    // 控制块在目标进程的地址
    LPVOID m_controlBlockRemoteAddr;

    DWORD  m_processID;
    HANDLE m_processHandle;

    // 保存哪些dll是因为hook所需才被加载的，析构的时候请一并释放
    std::vector<std::string> m_hookLoadDlls;
    DWORD                    m_loadLibraryAddr;
    DWORD                    m_freeLibraryAddr;

public:
    explicit ControlBlockManager(DWORD ProcessID);

    void fillSocketArgs();

    void injectControlBlock(HANDLE ProcessHandle);

    [[nodiscard]] LPVOID getRemoteControlBlockAddr() const noexcept;

    ~ControlBlockManager();
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H
