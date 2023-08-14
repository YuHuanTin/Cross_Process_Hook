

#ifndef CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H
#define CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H

#include <memory>
#include <Windows.h>

#include "../Utils/Utils.h"
#include "../DataStruct/DataStruct.h"
#include "../DllFuncRVAReader/ReadFromFile.h"

class ControlBlockBase {
public:
    explicit ControlBlockBase(DWORD ProcessID);

    virtual void fillControlBlockArgs() = 0;

    void injectControlBlock();

    [[nodiscard]] std::size_t getControlBlockRemoteAddr() const noexcept;

    std::unique_ptr<ControlBlock> &getControlBlockRef() noexcept;

    virtual ~ControlBlockBase();

protected:
    DWORD  m_processID;
    HANDLE m_processHandle = INVALID_HANDLE_VALUE;

    std::unique_ptr<ControlBlock>           m_controlBlock;      // 控制块的资源
    std::unique_ptr<RVAReaderBase>          m_rvaReader;         // rvaReader的资源
    std::unique_ptr<AutoDelete_AllocMemory> m_controlAddr;       // 控制块在目标进程的内存地址

    std::vector<std::string> m_hookLoadDlls;            // 保存需要释放的dll
    std::size_t              m_loadLibraryAddr = 0;
    std::size_t              m_freeLibraryAddr = 0;
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H
