

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
    LPVOID m_controlBlockRemoteAddr = nullptr;          // 控制块在目标进程的地址

    std::unique_ptr<ControlBlock>  m_controlBlock;      // 控制块的资源
    std::unique_ptr<RVAReaderBase> m_rvaReader;         // rvaReader的资源

    std::vector<std::string> m_hookLoadDlls;            // 保存需要释放的dll
    DWORD                    m_loadLibraryAddr = 0;
    DWORD                    m_freeLibraryAddr = 0;
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H
