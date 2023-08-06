

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

    // ������Щdll����Ϊhook����ű����صģ�������ʱ����һ���ͷ�
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
