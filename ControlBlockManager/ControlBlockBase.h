

#ifndef CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H
#define CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H

#include <memory>
#include <Windows.h>

#include "../DataStruct/DataStruct.h"
#include "../DllFuncRVAReader/ReadFromFile.h"

class ControlBlockBase {
public:
    explicit ControlBlockBase(DWORD ProcessID);

    virtual void fillControlBlockArgs() = 0;

    void injectControlBlock();

    LPVOID getControlBlockRemoteAddr() const noexcept;

    virtual ~ControlBlockBase();

protected:
    DWORD  m_processID;
    HANDLE m_processHandle;
    LPVOID m_controlBlockRemoteAddr = nullptr;          // ���ƿ���Ŀ����̵ĵ�ַ

    std::unique_ptr<ControlBlock>  m_controlBlock;      // ���ƿ����Դ
    std::unique_ptr<RVAReaderBase> m_rvaReader;         // rvaReader����Դ

    std::vector<std::string> m_hookLoadDlls;            // ������Ҫ�ͷŵ�dll
    DWORD                    m_loadLibraryAddr = 0;
    DWORD                    m_freeLibraryAddr = 0;
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H
