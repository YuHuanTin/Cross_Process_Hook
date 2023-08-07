

#ifndef CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H
#define CROSS_PROCESS_HOOK_CONTROLBLOCKMANAGER_H

#include <memory>
#include <optional>
#include <Windows.h>

#include "Data_Struct.h"
#include "../DllFuncRVAReader/ReadFromFile.h"

class ControlBlockManager {
private:
    // ���ƿ��ָ��
    std::unique_ptr<ControlBlock>  m_controlBlockPtr;
    // rva reader ָ��
    std::unique_ptr<RVAReaderBase> m_rvaReader;

    // ���ƿ���Ŀ����̵ĵ�ַ
    LPVOID m_controlBlockRemoteAddr;

    DWORD  m_processID;
    HANDLE m_processHandle;

    // ������Щdll����Ϊhook����ű����صģ�������ʱ����һ���ͷ�
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
