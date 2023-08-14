

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

    std::unique_ptr<ControlBlock>           m_controlBlock;      // ���ƿ����Դ
    std::unique_ptr<RVAReaderBase>          m_rvaReader;         // rvaReader����Դ
    std::unique_ptr<AutoDelete_AllocMemory> m_controlAddr;       // ���ƿ���Ŀ����̵��ڴ��ַ

    std::vector<std::string> m_hookLoadDlls;            // ������Ҫ�ͷŵ�dll
    std::size_t              m_loadLibraryAddr = 0;
    std::size_t              m_freeLibraryAddr = 0;
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCKBASE_H
